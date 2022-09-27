#pragma once

#include "connection.h"

namespace hc {
namespace net {
namespace ssl {

    class server_connection;

    typedef std::shared_ptr<server_connection> server_conn_ptr;
    typedef std::weak_ptr<server_connection> server_conn_hdl;

    class server_connection : public connection {
        public:
            server_connection(unique_ptr<SSL> ssl)
                : connection(std::move(ssl)), m_timeout_disabled(false), m_needs_close(false), m_locked(false)
            {}

            ~server_connection() {}

            bool handshake();

            void close();

            void set_ip(const std::string& ip) { m_ip = ip; }
            const std::string& get_ip() { return m_ip; }

            int time_since_create();
            void reset_timer();

            void set_timeout_disabled() { m_timeout_disabled = true; }

            void lock() { m_locked = true; }
            void unlock() { m_locked = false; }

            static server_conn_ptr conn_from_hdl(server_conn_hdl hdl);

        private:
            std::string m_ip;

            friend class tls_server;

            std::chrono::time_point<std::chrono::high_resolution_clock> m_create_time;

            bool m_timeout_disabled;

            bool m_needs_close;

            int m_close_event_fd;
            int m_associated_timer_fd;

            std::atomic<bool> m_locked;

    };

}
}
}