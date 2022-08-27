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
            std::unique_ptr<hc::net::ssl::tls_connection> connect(const std::string& host, const std::string& port);

        private:
            hc::net::ssl::unique_ptr<SSL_CTX> m_sslCtx;
    };

}
}
}