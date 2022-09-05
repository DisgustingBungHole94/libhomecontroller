#pragma once

#include "homecontroller/net/ssl/ssl.h"

#include <chrono>

namespace hc {
namespace net {
namespace ssl {

    class tls_connection;

    typedef std::shared_ptr<tls_connection> connection_ptr;
    typedef std::weak_ptr<tls_connection> connection_hdl;

    enum class tls_connection_mode {
        SERVER, CLIENT
    };

    class tls_connection {
        public:
            tls_connection(unique_ptr<SSL> ssl, tls_connection_mode mode)
                : m_ssl(std::move(ssl)), m_closed(false), m_ready(false), m_mode(mode)
            {
                m_create_time = std::chrono::high_resolution_clock::now();
            }

            ~tls_connection() {}

            void handshake();

            std::string recv();
            void send(const std::string& data);

            void close();

            bool ready() { return m_ready; }
            bool closed() { return m_closed; }

            int get_socket() { return SSL_get_fd(m_ssl.get()); }

            void set_ip(const std::string& ip) { m_ip = ip; }
            const std::string& get_ip() { return m_ip; }

            void set_uri(const std::string& uri) { m_uri = uri; }
            const std::string& get_uri() { return m_uri; }

            int time_since_create();

            static connection_ptr conn_from_hdl(connection_hdl hdl);

        private:
            void _close();

            unique_ptr<SSL> m_ssl;

            std::string m_ip;
            std::string m_uri;

            bool m_ready;
            bool m_closed;

            tls_connection_mode m_mode;

            std::chrono::time_point<std::chrono::high_resolution_clock> m_create_time;

        // tls server related data
        private:
            friend class tls_server;

            bool m_needs_close;

            int m_close_event_fd;
            int m_associated_timer_fd;
    };

}
}
}