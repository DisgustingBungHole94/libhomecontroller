#pragma once

#include "homecontroller/util/logger.h"
#include "homecontroller/net/ssl/tls_client.h"

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
            typedef std::shared_ptr<hc::net::ssl::tls_connection> connection_ptr;
            typedef std::weak_ptr<hc::net::ssl::tls_connection> connection_hdl;

            //typedef std::function<void(connection_hdl)> callback;

            tls_server() 
                : m_logger("tls_server")
            {}
            
            ~tls_server() {}

            void init(int port, const std::string& cert_file, const std::string& priv_key_file, int default_timeout);
            
            void run();
            void stop();

            /*
            void set_connect_callback(callback connect_callback) { m_connect_callback = connect_callback; }
            void set_data_callback(callback data_callback) { m_data_callback = data_callback; }
            void set_disconnect_callback(callback disconnect_callback) { m_disconnect_callback = disconnect_callback; }
            */

            virtual void on_connect(connection_hdl conn);
            virtual void on_data(connection_hdl conn);
            virtual void on_disconnect(connection_hdl conn);

            static connection_ptr conn_from_hdl(connection_hdl hdl);

        private:
            struct epoll_fd_data {
                int m_fd;
                
                enum class type {
                    SOCKET, TIMER, OTHER
                } m_type;

                connection_hdl m_conn_hdl;
            };

            void accept();

            void handle_data(epoll_fd_data* data_ptr);
            void handle_timeout(epoll_fd_data* data_ptr);

            void handle_close();

            std::string get_socket_ip(struct sockaddr_in* client_addr);

            int create_timer_fd();
            void update_timer_fd(int fd);

            void epoll_ctl_add(int fd, uint32_t events, epoll_fd_data::type type, connection_ptr conn_ptr);
            void epoll_ctl_add(int fd, uint32_t events);

            void epoll_ctl_del(int fd);

            void set_nonblocking(int fd);

            hc::util::logger m_logger;

            int m_socket;
            int m_epfd;

            int m_close_event_fd;

            hc::net::ssl::unique_ptr<SSL_CTX> m_ssl_ctx;

            std::map<int, connection_ptr> m_connections;
            std::map<int, epoll_fd_data*> m_epoll_fds;

            /*
            callback m_connect_callback;
            callback m_data_callback;
            callback m_disconnect_callback;
            */

            struct itimerspec m_timeout;

            bool m_running;
    };

}
}
}

