#include "homecontroller/ws/ws_client.h"

#include "homecontroller/exception/exception.h"

namespace hc {
namespace ws {
    
    void websocket_client::init() {
        try {
            m_client.set_access_channels(websocketpp::log::alevel::none);
            m_client.clear_access_channels(websocketpp::log::alevel::frame_payload);
            m_client.set_error_channels(websocketpp::log::elevel::none);
        } catch(std::exception& e) {
            throw exception("failed to initialize WebSocket client: " + std::string(e.what()), "hc::ws::websocket_server::init");
        }
    }
    
    std::unique_ptr<client_connection> websocket_client::connect(net::ssl::connection_hdl tls_conn_hdl) {        
        std::error_code ec;
        
        net::ssl::connection_ptr conn = net::ssl::tls_connection::conn_from_hdl(tls_conn_hdl);

        // websocketpp connection (used internally to parse messages)
        client::connection_ptr ws_conn_ptr = m_client.get_connection(conn->get_uri(), ec);
        if (ec) {
            throw exception("failed to create connection: " + ec.message(), "hc::ws::websocket_client::connect");
        }

        std::unique_ptr<client_connection> ws_conn = std::make_unique<client_connection>(ws_conn_ptr, tls_conn_hdl);
        return std::move(ws_conn);
    }

}
}