#include "homecontroller/ws/ws_server.h"

#include "homecontroller/exception/exception.h"

namespace hc {
namespace ws {

    void websocket_server::init() {
        try {
            m_server.set_access_channels(websocketpp::log::alevel::none);
            m_server.clear_access_channels(websocketpp::log::alevel::frame_payload);
            m_server.set_error_channels(websocketpp::log::elevel::none);
        } catch(std::exception& e) {
            throw exception("failed to initialize WebSocket server: " + std::string(e.what()), "hc::ws::websocket_server::init");
        }
    }

    std::unique_ptr<server_connection> websocket_server::connect(net::ssl::connection_hdl tls_conn_hdl) {
        server::connection_ptr ws_conn_ptr;
        
        try {
            ws_conn_ptr = m_server.get_connection();
        } catch(std::exception& e) {
            throw exception("failed to create connection: " + std::string(e.what()), "hc::ws::websocket_server::connect");
        }

        std::unique_ptr<server_connection> conn = std::make_unique<server_connection>(ws_conn_ptr, tls_conn_hdl);
        return std::move(conn);
    }

}
}