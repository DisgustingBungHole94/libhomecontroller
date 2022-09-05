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
            std::unique_ptr<client_connection> connect(net::ssl::connection_hdl tls_conn_hdl);

        private:
            client m_client;
    };

}
}