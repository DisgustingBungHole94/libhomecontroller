#include "homecontroller/net/ssl/tls_server.h"

#include "homecontroller/exception/exception.h"

#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

#include <iostream>
#include <thread>

namespace hc {
namespace net {
namespace ssl {

    void tls_server::init(int port, const std::string& cert_file, const std::string& priv_key_file, int default_timeout) {
        m_ssl_ctx.reset(SSL_CTX_new(TLS_server_method()));
        SSL_CTX_set_min_proto_version(m_ssl_ctx.get(), TLS1_2_VERSION);

        if (SSL_CTX_use_certificate_file(m_ssl_ctx.get(), cert_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
            m_logger.err("openssl error: " + error_str());
            throw exception("failed to load server certificate file", "hc::net::ssl::tls_server::init");
        }

        if (SSL_CTX_use_PrivateKey_file(m_ssl_ctx.get(), priv_key_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
            m_logger.err("openssl error: " + error_str());
            throw exception("failed to load server private key file", "hc::net::ssl::tls_server::init");
        }

        m_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_socket < 0) {
            throw exception("failed to create socket", "hc::net::ssl::tls_server::init");
        }

        const int reuse_addr = 1;
        setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(int));

        struct sockaddr_in server_addr;
        std::memset(&server_addr, 0, sizeof(server_addr));

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(m_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            throw exception("failed to bind socket, is the port in use?", "hc::net::ssl::tls_server::init");
        }

        bool err = false;
        std::string err_msg;

        if ((m_epfd = epoll_create(1)) == -1) {
            err = true;
            err_msg = "failed to create epoll descriptor";
        }

        else if ((m_close_event_fd = eventfd(0, 0)) == -1) {
            err = true;
            err_msg = "failed to create exit event descriptor";
        }

        else if (!set_nonblocking(m_socket) || !set_nonblocking(m_close_event_fd)) {
            err = true;
            err_msg = "failed to make fds nonblocking";
        }

        else if (!epoll_ctl_add(m_close_event_fd, EPOLLIN) || !epoll_ctl_add(m_socket, EPOLLIN | EPOLLOUT)) {
            err = true;
            err_msg = "failed to add fds to epoll list";
        }
        
        if (err) {
            if (m_socket != -1)         ::close(m_socket);
            if (m_epfd != -1)           ::close(m_epfd);
            if (m_close_event_fd != -1) ::close(m_close_event_fd);

            throw exception(err_msg, "hc::net::ssl::tls_server::init");
        }

        listen(m_socket, 16);

        m_timeout.it_interval.tv_sec = 0;
        m_timeout.it_interval.tv_nsec = 0;
        m_timeout.it_value.tv_sec = default_timeout / 1000;
        m_timeout.it_value.tv_nsec = (default_timeout % 1000) * 1000000;

        m_running = true;

        m_logger.log("server listening on port " + std::to_string(port));
    }
    
    void tls_server::run() {
        static const int MAX_EVENTS = 32;
        struct epoll_event events[MAX_EVENTS];

        while(m_running) {
            // wait for event
            int num_fds = epoll_wait(m_epfd, events, MAX_EVENTS, -1);

            // iterate through all fds that are ready
            for (int i = 0; i < num_fds; i++) {
                epoll_fd_data* data_ptr = static_cast<epoll_fd_data*>(events[i].data.ptr);

                // check if event was on the close event fd
                if (data_ptr->m_fd == m_close_event_fd) {
                    // check if close event was to close the server
                    if (!m_running) {
                        break;
                    }

                    handle_close();
                }

                // if socket is the server socket, accept new client
                else if (data_ptr->m_fd == m_socket) {
                    accept();
                } 

                else {
                    
                    // otherwise, find socket in connection list and call on_data() callback
                    if (events[i].events & EPOLLIN) {
                        switch(data_ptr->m_type) {
                            case epoll_fd_data::type::SOCKET:
                                handle_data(data_ptr);
                                break;
                            case epoll_fd_data::type::TIMER:
                                handle_timeout(data_ptr);
                                break;
                            default:
                                m_logger.err("data received from unknown fd");
                                break;
                        }
                    }

                    // check if connection was closed by remote peer
                    if (events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
                        // close socket
                        try {
                            auto conn_ptr = server_connection::conn_from_hdl(data_ptr->m_conn_hdl);
                            close_connection(conn_ptr);
                        } catch(exception& e) {
                            m_logger.err("failed to close socket, bad handle");
                            epoll_ctl_del(data_ptr->m_fd);
                        }
                    }

                }
            }
        }

        // close server socket
        shutdown(m_socket, SHUT_RDWR);
        ::close(m_socket);

        // close epoll descriptor
        ::close(m_epfd);

        // close client sockets

        int num_closed = 0;
        for (auto& x : m_connections) {
            x.second->_close();
            if (x.second->ready()) {
                on_disconnect(x.second);
            }

            num_closed++;
        }

        m_logger.dbg("disconnected " + std::to_string(num_closed) + " clients");

        for (auto& x : m_epoll_fds) {
            delete x.second;
        }

        m_connections.clear();
        m_epoll_fds.clear();

        m_logger.log("server closed");
    }

    void tls_server::stop() {
        m_running = false;

        uint64_t d = 1;

        int numBytes = write(m_close_event_fd, &d, sizeof(uint64_t));
        if (numBytes != sizeof(uint64_t)) {
            throw exception("failed to write to close event fd", "hc::net::ssl::tls_server::stop");
        }
    }

    void tls_server::on_connect(server_conn_hdl conn) {}
    void tls_server::on_data(server_conn_hdl conn) {}
    void tls_server::on_disconnect(server_conn_hdl conn) {}

    void tls_server::accept() {
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);

        int client_socket = ::accept(m_socket, (struct sockaddr*)&client_addr, (socklen_t*)&client_len);
        if (client_socket == -1) {
            m_logger.err("failed to establish connection");
            return;
        }

        unique_ptr<SSL> client_ssl(SSL_new(m_ssl_ctx.get()));
        SSL_set_fd(client_ssl.get(), client_socket);

        std::shared_ptr<server_connection> conn_ptr = std::make_shared<server_connection>(std::move(client_ssl));
        conn_ptr->set_ip(get_socket_ip(&client_addr));

        bool err = false;
        
        int timer_fd = create_timer_fd();

        if (timer_fd == -1) {
            err = true;
            m_logger.err("failed to create timer fd");
        }

        else if (!set_nonblocking(timer_fd) || !set_nonblocking(client_socket)) {
            err = true;
            m_logger.err("failed to set fds to nonblocking");
        }

        else if (!epoll_ctl_add(timer_fd, EPOLLIN, epoll_fd_data::type::TIMER, conn_ptr)) {
            err = true;
            m_logger.err("failed to add timer fd to epoll list");
        }

        else if (!epoll_ctl_add(client_socket, EPOLLIN | EPOLLRDHUP | EPOLLHUP, epoll_fd_data::type::SOCKET, conn_ptr)) {
            epoll_ctl_del(timer_fd);

            err = true;
            m_logger.err("failed to add client socket to epoll list");
        }

        if (err) {
            if (timer_fd != -1) { ::close(timer_fd); }
            if (client_socket != -1) { ::close(client_socket); }

            return;
        }

        conn_ptr->m_close_event_fd = m_close_event_fd;
        conn_ptr->m_associated_timer_fd = timer_fd;

        m_connections.insert(std::make_pair(client_socket, conn_ptr));

        m_logger.dbg("client [" + conn_ptr->get_ip() + "] connected");
    }

    void tls_server::handle_data(epoll_fd_data* data_ptr) {
        server_conn_ptr conn_ptr;
        try {
            conn_ptr = server_connection::conn_from_hdl(data_ptr->m_conn_hdl);
        } catch(exception& e) {
            m_logger.err("failed to handle data, bad handle");

            epoll_ctl_del(data_ptr->m_fd);
            return;
        }

        if (conn_ptr->m_locked) {
            return;
        }

        if (!conn_ptr->m_timeout_disabled && !update_timer_fd(conn_ptr->m_associated_timer_fd)) {
            m_logger.err("failed to update timer associated with connection");
        }

        // check if tls is initialized
        if (!conn_ptr->ready()) {
            perform_tls_handshake(conn_ptr);
        }
            
        // otherwise, call ready callback
        else {
            on_data(conn_ptr);
        }
    }
    
    void tls_server::handle_timeout(epoll_fd_data* data_ptr) {
        server_conn_ptr conn_ptr;
        bool conn_closed = false;

        try {
            conn_ptr = server_connection::conn_from_hdl(data_ptr->m_conn_hdl);
        } catch(exception& e) {
            conn_closed = true;
        }

        if (!conn_closed && !conn_ptr->m_timeout_disabled) {
            m_logger.dbg("client [" + conn_ptr->get_ip() + "] timed out");
            close_connection(conn_ptr);
        }

        epoll_ctl_del(data_ptr->m_fd);
        ::close(data_ptr->m_fd);
    }

    void tls_server::handle_close() {
        // read data from event fd
        uint64_t d;

        int num_bytes = read(m_close_event_fd, &d, sizeof(uint64_t));
        if (num_bytes != sizeof(uint64_t)) {
            m_logger.err("failed to read close event fd");
            return;
        }

        int num_closed = 0;

        for(auto mit = m_connections.cbegin(); mit != m_connections.cend() && num_closed < d; ) {
            if (mit->second->m_needs_close) {
                if (close_connection(mit->second, false)) {
                    mit = m_connections.erase(mit);
                } else {
                    ++mit;
                }

                num_closed++;
            } else {
                ++mit;
            }
        }

    }

    bool tls_server::close_connection(const server_conn_ptr& conn_ptr, bool delete_from_list) {
        // for multithread safety,
        // a connection can be locked, prohibiting it from closing
        //
        // the connection will try to close until it is unlocked (without blocking)
        //
        // this way, if a connection is being used in one thread and another closes it,
        // the connection can be kept open until it is no longer being used
        if (conn_ptr->m_locked) {
            conn_ptr->close();
            return false;
        }
        
        // delete from epoll list
        if (!epoll_ctl_del(conn_ptr->get_socket())) {
            m_logger.err("failed to remove connection from epoll list");
        }

        // close socket
        conn_ptr->_close();

        m_logger.dbg("client [" + conn_ptr->get_ip() + "] disconnected");

        // call close callback
        if (conn_ptr->ready()) {
            on_disconnect(conn_ptr);
        }

        if (delete_from_list) {
            m_connections.erase(conn_ptr->get_socket());
        }

        return true;
    }

    void tls_server::perform_tls_handshake(const server_conn_ptr& conn_ptr) {
        // perform tls handshake
        if (!conn_ptr->handshake()) {
            m_logger.err("openssl error: " + error_str());
            close_connection(conn_ptr);
        }
        
        // if handshake is complete, call connect callback
        else if (conn_ptr->ready()) {
            on_connect(conn_ptr);
        }
    }

    std::string tls_server::get_socket_ip(struct sockaddr_in* client_addr) {
        struct in_addr ip_addr = client_addr->sin_addr;
        char ip_buf[INET_ADDRSTRLEN];

        inet_ntop(AF_INET, &ip_addr, ip_buf, INET_ADDRSTRLEN);

        return std::string(ip_buf);
    }

    int tls_server::create_timer_fd() {
        int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
        if (tfd == -1) {
            return -1;
        }

        update_timer_fd(tfd);

        return tfd;
    }

    bool tls_server::update_timer_fd(int fd) {
        if (timerfd_settime(fd, 0, &m_timeout, nullptr) == -1) {
            return false;
        }

        return true;
    }

    bool tls_server::epoll_ctl_add(int fd, uint32_t events, epoll_fd_data::type type, const server_conn_ptr& conn_ptr) {
        epoll_fd_data* data = new epoll_fd_data();
        data->m_fd = fd;
        data->m_type = type;
        data->m_conn_hdl = conn_ptr;
        
        struct epoll_event ev;
        ev.events = events;
        ev.data.ptr = data;

        if (epoll_ctl(m_epfd, EPOLL_CTL_ADD, data->m_fd, &ev) == -1) {
            delete data;
            return false;
        }

        m_epoll_fds.insert(std::make_pair(fd, data));

        return true;
    }

    bool tls_server::epoll_ctl_add(int fd, uint32_t events) {
        return epoll_ctl_add(fd, events, epoll_fd_data::type::OTHER, nullptr);
    }

    bool tls_server::epoll_ctl_del(int fd) {
        auto mit = m_epoll_fds.find(fd);
        if (mit != m_epoll_fds.end()) {
            delete mit->second;
            m_epoll_fds.erase(mit);
        }

        if (epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
            return false;
        }

        return true;
    }

    bool tls_server::set_nonblocking(int fd) {
        if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) < 0) {
            return false;
        }

        return true;
    }

}
}
}