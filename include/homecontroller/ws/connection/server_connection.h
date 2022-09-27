#pragma once

#include "homecontroller/ws/ws_config.h"
#include "homecontroller/ws/connection/connection.h"

namespace hc {
namespace ws {

    class server_connection : public connection<server_config> {
        public:
            server_connection(ws_conn_ptr ws_conn_ptr, net::ssl::conn_hdl tls_conn_hdl, const std::string& upgrade_request)
                : connection(ws_conn_ptr, tls_conn_hdl), m_upgrade_request(upgrade_request)
            {}

            ~server_connection() {}

            void start();

        private:
            std::string m_upgrade_request;

    };

    typedef std::shared_ptr<server_connection> server_conn_ptr;

}
}