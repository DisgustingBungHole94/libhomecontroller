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
            throw hc::exception("failed to initialize WebSocket server: " + std::string(e.what()), "hc::ws::websocket_server::init");
        }
    }

    std::unique_ptr<hc::ws::server_connection> websocket_server::connect(std::unique_ptr<hc::net::ssl::tls_connection> conn) {
        hc::ws::server::connection_ptr wppConn;
        
        try {
            wppConn = m_server.get_connection();
        } catch(std::exception& e) {
            throw hc::exception("failed to create connection: " + std::string(e.what()), "hc::ws::websocket_server::connect");
        }

        std::unique_ptr<hc::ws::server_connection> wsConn = std::make_unique<hc::ws::server_connection>(wppConn, std::move(conn));
        return std::move(wsConn);
    }

}
}