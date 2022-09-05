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

        m_epfd = epoll_create(1);
        if (m_epfd == -1) {
            throw exception("failed to create epoll descriptor", "hc::net::ssl::tls_server::init");
        }

        m_close_event_fd = eventfd(0, 0);
        if (m_close_event_fd == -1) {
            throw exception("failed to create exit event descriptor", "hc::net::ssl::tls_server::init");
        }

        set_nonblocking(m_socket);
        set_nonblocking(m_close_event_fd);

        epoll_ctl_add(m_close_event_fd, EPOLLIN);
        epoll_ctl_add(m_socket, EPOLLIN | EPOLLOUT | EPOLLET);
        
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
            int num_fds = epoll_wait(m_epfd, events, MAX_EVENTS, 5);

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

                // check if connection was closed by remote peer
                else if (events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
                    // close socket
                    try {
                        tls_connection::conn_from_hdl(data_ptr->m_conn_hdl)->close();
                    } catch(exception& e) {
                        m_logger.err("failed to close socket, bad handle");

                        epoll_ctl_del(data_ptr->m_fd);
                    }
                }
                
                // otherwise, find socket in connection list and call onReady() callback
                else if (events[i].events & EPOLLIN) {
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
            on_disconnect(x.second);

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

    void tls_server::on_connect(connection_hdl conn) {}
    void tls_server::on_data(connection_hdl conn) {}
    void tls_server::on_disconnect(connection_hdl conn) {}

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

        std::shared_ptr<tls_connection> conn = std::make_shared<tls_connection>(std::move(client_ssl), tls_connection_mode::SERVER);
        conn->set_ip(get_socket_ip(&client_addr));

        int timer_fd = create_timer_fd();

        if (timer_fd == -1) {
            m_logger.err("failed to create timer fd");
            return;
        }

        if (!set_nonblocking(timer_fd) || !set_nonblocking(client_socket)) {
            ::close(timer_fd);
            ::close(client_socket);

            m_logger.err("failed to set fds to nonblocking");
            return;
        }

        int events = EPOLLIN;
        if (!epoll_ctl_add(timer_fd, EPOLLIN, epoll_fd_data::type::TIMER, conn)) {
            ::close(timer_fd);
            ::close(client_socket);

            m_logger.err("failed to add timer fd to epoll list");
            return;
        }

        events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP;
        if (!epoll_ctl_add(client_socket, events,epoll_fd_data::type::SOCKET, conn)) {
            ::close(timer_fd);
            ::close(client_socket);

            epoll_ctl_del(timer_fd);

            m_logger.err("failed to add client socket to epoll list");
            return;
        }

        conn->m_close_event_fd = m_close_event_fd;
        conn->m_associated_timer_fd = timer_fd;

        m_connections.insert(std::make_pair(client_socket, conn));
    }

    void tls_server::handle_data(epoll_fd_data* data_ptr) {
        connection_ptr conn;
        try {
            conn = tls_connection::conn_from_hdl(data_ptr->m_conn_hdl);
        } catch(exception& e) {
            m_logger.err("failed to handle data, bad handle");

            epoll_ctl_del(data_ptr->m_fd);
            return;
        }


        if (!update_timer_fd(conn->m_associated_timer_fd)) {
            m_logger.err("failed to update timer associated with connection");
        }

        // check if tls is initialized
        if (!conn->ready()) {
            // if not, perform tls handshake
            conn->handshake();

            // check if handshake failed
            if (conn->closed()) {
                m_logger.err("openssl error: " + error_str());
            }
            
            // if handshake is complete, call connect callback
            else if (conn->ready()) {
                on_connect(conn);
            }
        }
            
        // otherwise, call ready callback
        else {
            on_data(conn);
        }
    }
    
    void tls_server::handle_timeout(epoll_fd_data* data_ptr) {
        connection_ptr conn;
        
        bool conn_closed = false;

        try {
            conn = tls_connection::conn_from_hdl(data_ptr->m_conn_hdl);
        } catch(exception& e) {
            conn_closed = true;
        }

        if (!conn_closed) {
            m_logger.dbg("client [" + conn->get_ip() + "] timed out");
            conn->close();
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
        for (auto mit = m_connections.cbegin(), next_mit = mit; mit != m_connections.cend() || num_closed < d; mit = next_mit) {            
            ++next_mit;

            if (mit->second->m_needs_close) {
                m_logger.dbg("client [" + mit->second->get_ip() + "] disconnected");

                // delete from epoll list
                epoll_ctl_del(mit->first);

                // close socket
                mit->second->_close();

                // call close callback
                on_disconnect(mit->second);

                // delete from connection list
                mit = m_connections.erase(mit);

                num_closed++;
            }
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
            //throw exception("failed to create timer fd", "hc::net::ssl::tls_server::create_timer_fd");
        }

        update_timer_fd(tfd);

        return tfd;
    }

    bool tls_server::update_timer_fd(int fd) {
        if (timerfd_settime(fd, 0, &m_timeout, nullptr) == -1) {
            return false;
            //throw exception("failed to set timeout", "hc::net::ssl::tls_server::create_timer_fd");
        }

        return true;
    }

    bool tls_server::epoll_ctl_add(int fd, uint32_t events, epoll_fd_data::type type, connection_ptr conn_ptr) {
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
            //throw exception("failed to add fd to epoll list", "hc::net::ssl::tls_server::epollCtlAdd");
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
            //throw exception("failed to remove fd from epoll list", "hc::net::ssl::tls_server::epoll_ctl_del");
        }

        return true;
    }

    bool tls_server::set_nonblocking(int fd) {
        if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) < 0) {
            return false;
            //throw exception("failed to set socket to nonblocking", "hc::net::ssl::tls_server::set_nonblocking");
        }

        return true;
    }

}
}
}