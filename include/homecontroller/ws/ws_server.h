#pragma once

#include "homecontroller/ws/ws_config.h"
#include "homecontroller/ws/connection/server_connection.h"

#include "homecontroller/net/ssl/connection/server_connection.h"

namespace hc {
namespace ws {

    class ws_server {
        public:
            ws_server();
            ~ws_server() {}

            server_conn_ptr connect(net::ssl::server_conn_hdl tls_conn_hdl, const std::string& upgrade_request);
    
        private:
            server m_server;

    };

}
}