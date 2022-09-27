#pragma once

#include "homecontroller/util/logger.h"
#include "homecontroller/net/ssl/connection/server_connection.h"

#include <netinet/in.h>
#include <sys/timerfd.h>

#include <functional>
#include <map>
#include <memory>
#include <mutex>

namespace hc {
namespace net {
namespace ssl {

    class tls_server {
        public:
            tls_server() 
                : m_logger("tls_server")
            {}
            
            ~tls_server() {}

            void init(int port, const std::string& cert_file, const std::string& priv_key_file, int default_timeout);
            
            void run();
            void stop();

            virtual void on_connect(server_conn_hdl conn);
            virtual void on_data(server_conn_hdl conn);
            virtual void on_disconnect(server_conn_hdl conn);

        private:
            struct epoll_fd_data {
                int m_fd;
                
                enum class type {
                    SOCKET, TIMER, OTHER
                } m_type;

                server_conn_hdl m_conn_hdl;
            };

            void accept();

            void handle_data(epoll_fd_data* data_ptr);
            void handle_timeout(epoll_fd_data* data_ptr);
            void handle_close();

            bool close_connection(const server_conn_ptr& conn_ptr, bool delete_from_list = true);
            void perform_tls_handshake(const server_conn_ptr& conn_ptr);

            std::string get_socket_ip(struct sockaddr_in* client_addr);

            int create_timer_fd();
            bool update_timer_fd(int fd);

            bool epoll_ctl_add(int fd, uint32_t events, epoll_fd_data::type type, const server_conn_ptr& conn_ptr);
            bool epoll_ctl_add(int fd, uint32_t events);

            bool epoll_ctl_del(int fd);

            bool set_nonblocking(int fd);

            util::logger m_logger;

            int m_socket;
            int m_epfd;

            int m_close_event_fd;

            unique_ptr<SSL_CTX> m_ssl_ctx;

            std::map<int, server_conn_ptr> m_connections;
            std::map<int, epoll_fd_data*> m_epoll_fds;

            struct itimerspec m_timeout;

            bool m_running;
    };

}
}
}

