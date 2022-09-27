#pragma once

#include "homecontroller/net/ssl/connection/client_connection.h"

namespace hc {
namespace net {
namespace ssl {
    
    class tls_client {
        public:
            tls_client() {}
            ~tls_client() {}

            void init();
            client_conn_ptr connect(const std::string& host, const std::string& port);

        private:
            unique_ptr<SSL_CTX> m_ssl_ctx;
    };

}
}
}