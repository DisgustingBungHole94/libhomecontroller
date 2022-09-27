#pragma once

#include "homecontroller/ws/ws_config.h"
#include "homecontroller/ws/connection/client_connection.h"

#include "homecontroller/net/ssl/tls_client.h"

namespace hc {
namespace ws {

    class ws_client {
        public:
            ws_client();
            ~ws_client() {}

            client_conn_ptr connect(net::ssl::client_conn_hdl tls_conn_hdl);

        private:
            client m_client;
    };

}
}