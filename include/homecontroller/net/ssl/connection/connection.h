#pragma once

#include "homecontroller/net/ssl/ssl.h"

#include <chrono>
#include <atomic>

namespace hc {
namespace net {
namespace ssl {

    class connection;

    typedef std::shared_ptr<connection> conn_ptr;
    typedef std::weak_ptr<connection> conn_hdl;

    class connection {
        public:
            connection(unique_ptr<SSL> ssl)
                : m_ssl(std::move(ssl)), m_closed(false), m_ready(false)
            {}

            ~connection() {}

            virtual bool handshake();

            std::string recv();
            void send(const std::string& data);

            virtual void close();

            bool ready() { return m_ready; }
            bool closed() { return m_closed; }

            int get_socket() { return SSL_get_fd(m_ssl.get()); }

            static conn_ptr conn_from_hdl(conn_hdl hdl);

        protected:
            void _close();

            unique_ptr<SSL> m_ssl;

            bool m_ready;
            bool m_closed;

    };

}
}
}