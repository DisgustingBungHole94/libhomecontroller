#pragma once

#include "homecontroller/ws/ws_config.h"
#include "homecontroller/ws/ws_connection.h"

#include "homecontroller/net/ssl/tls_client.h"

namespace hc {
namespace ws {

    class websocket_server {
        public:
            websocket_server() {}

            ~websocket_server() {}

            void init();
            std::unique_ptr<hc::ws::server_connection> connect(std::unique_ptr<hc::net::ssl::tls_connection> conn);
    
        private:
            hc::ws::server m_server;

    };

}
}