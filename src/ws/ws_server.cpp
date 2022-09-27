#include "homecontroller/ws/ws_server.h"

#include "homecontroller/exception/exception.h"

namespace hc {
namespace ws {

    ws_server::ws_server() {
        m_server.set_access_channels(websocketpp::log::alevel::none);
        m_server.clear_access_channels(websocketpp::log::alevel::frame_payload);
        m_server.set_error_channels(websocketpp::log::elevel::none);
    }

    server_conn_ptr ws_server::connect(net::ssl::server_conn_hdl tls_conn_hdl, const std::string& upgrade_request) {
        auto tls_conn_ptr = hc::net::ssl::server_connection::conn_from_hdl(tls_conn_hdl);
        tls_conn_ptr->set_timeout_disabled();
        
        server::connection_ptr ws_conn_ptr = m_server.get_connection();

        server_conn_ptr conn = std::make_shared<server_connection>(ws_conn_ptr, tls_conn_hdl, upgrade_request);

        return conn;
    }

}
}