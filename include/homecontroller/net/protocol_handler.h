#pragma once

#include "homecontroller/net/ssl/tls_connection.h"

#include <memory>

namespace hc {
namespace net {

    class protocol_handler {
        public:
            protocol_handler(std::unique_ptr<hc::net::ssl::tls_connection> conn) 
                : m_conn(std::move(conn)), m_finished(false)
            {}

            ~protocol_handler() {}

            virtual void start();
            virtual void stop();

            virtual void onReady();

            bool finished() { return m_finished; }

            hc::net::ssl::tls_connection* getConnection() { return m_conn.get(); }

        protected:
            std::unique_ptr<hc::net::ssl::tls_connection> m_conn;

            // set when connection should be closed
            bool m_finished;
    };

}
}