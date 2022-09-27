#pragma once

#include "homecontroller/ws/ws_config.h"
#include "homecontroller/ws/connection/connection.h"

namespace hc {
namespace ws {
    
    class client_connection : public connection<client_config> {
        public:
            client_connection(ws_conn_ptr ws_conn_ptr, net::ssl::conn_hdl tls_conn_hdl)
                : connection(ws_conn_ptr, tls_conn_hdl)
            {}

            ~client_connection() {}

            void start();
    };

    typedef std::shared_ptr<client_connection> client_conn_ptr;

}
}