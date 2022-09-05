#pragma once

#include "homecontroller/net/ssl/tls_connection.h"

namespace hc {
namespace net {
namespace ssl {
    
    class tls_client {
        public:
            tls_client() {}
            ~tls_client() {}

            void init();
            void connect(const std::string& host, const std::string& port);

            connection_ptr get_connection() { return m_conn; }

        private:
            unique_ptr<SSL_CTX> m_sslCtx;

            connection_ptr m_conn;       
    };

}
}
}