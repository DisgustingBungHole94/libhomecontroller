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
            throw hc::exception("failed to initialize WebSocket client: " + std::string(e.what()), "hc::ws::websocket_server::init");
        }
    }
    
    std::unique_ptr<hc::ws::client_connection> websocket_client::connect(std::unique_ptr<hc::net::ssl::tls_connection> conn) {        
        std::error_code ec;
        
        hc::ws::client::connection_ptr wppConn = m_client.get_connection(conn->get_uri(), ec);
        if (ec) {
            throw hc::exception("failed to create connection: " + ec.message(), "hc::ws::websocket_client::connect");
        }

        std::unique_ptr<hc::ws::client_connection> wsConn = std::make_unique<hc::ws::client_connection>(wppConn, std::move(conn));
        return std::move(wsConn);
    }

}
}