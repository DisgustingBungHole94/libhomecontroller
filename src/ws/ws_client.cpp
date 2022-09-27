#include "homecontroller/ws/ws_client.h"

#include "homecontroller/exception/exception.h"

namespace hc {
namespace ws {
    
    ws_client::ws_client() {
        m_client.set_access_channels(websocketpp::log::alevel::none);
        m_client.clear_access_channels(websocketpp::log::alevel::frame_payload);
        m_client.set_error_channels(websocketpp::log::elevel::none);
    }
    
    client_conn_ptr ws_client::connect(net::ssl::client_conn_hdl tls_conn_hdl) {                
        auto tls_conn_ptr = net::ssl::client_connection::conn_from_hdl(tls_conn_hdl);

        std::error_code ec;
        client_connection::ws_conn_ptr ws_conn_ptr = m_client.get_connection(tls_conn_ptr->get_uri(), ec);
        if (ec) {
            throw exception("failed to create connection: " + ec.message(), "hc::ws::websocket_client::connect");
        }

        client_conn_ptr conn = std::make_shared<client_connection>(ws_conn_ptr, tls_conn_hdl);
        return conn;
    }

}
}