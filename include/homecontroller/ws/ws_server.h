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
            std::unique_ptr<server_connection> connect(net::ssl::connection_hdl tls_conn_hdl);
    
        private:
            server m_server;

    };

}
}