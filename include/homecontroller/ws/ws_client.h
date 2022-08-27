#pragma once

#include "homecontroller/ws/ws_config.h"
#include "homecontroller/ws/ws_connection.h"

#include "homecontroller/net/ssl/tls_client.h"

namespace hc {
namespace ws {

    class websocket_client {
        public:
            websocket_client() {}

            ~websocket_client() {}

            void init();
            std::unique_ptr<hc::ws::client_connection> connect(std::unique_ptr<hc::net::ssl::tls_connection> conn);

        private:
            hc::ws::client m_client;
    };

}
}