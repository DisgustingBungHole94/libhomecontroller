#pragma once

#include "homecontroller/ws/ws_config.h"
#include "homecontroller/net/protocol_handler.h"
#include "homecontroller/util/logger.h"

#include <functional>
#include <vector>

namespace hc {
namespace ws {

    template<typename config>
    class websocket_connection : public hc::net::protocol_handler {
        public:
            typedef std::shared_ptr<websocketpp::connection<config>> conn_ptr;

            websocket_connection(conn_ptr connectionPtr, std::unique_ptr<hc::net::ssl::tls_connection> conn) 
                : m_logger("WS@" + conn->get_ip()), m_wsConnection(connectionPtr), protocol_handler(std::move(conn))
            {}

            ~websocket_connection() {}

            void upgrade(const std::string& upgradeRequest);

            virtual void start();
            virtual void stop();

            virtual void onReady();

            void send(const std::vector<uint8_t>& msg);
            void close(const std::string& reason);

            void setMessageCallback(const std::function<void(std::vector<uint8_t>)>& callback) { m_messageCallback = callback; }

        private:
            std::error_code onVectorWrite(websocketpp::connection_hdl hdl, std::vector<websocketpp::transport::buffer> const& bufs);
            std::error_code onWrite(websocketpp::connection_hdl hdl, const char* data, std::size_t len);

            void onOpen(websocketpp::connection_hdl hdl);
            void onFail(websocketpp::connection_hdl hdl);
            void onClose(websocketpp::connection_hdl hdl);
            void onMessage(websocketpp::connection_hdl hdl, hc::ws::server::message_ptr msg);

            hc::util::logger m_logger;

            std::function<void(std::vector<uint8_t>)> m_messageCallback;

            conn_ptr m_wsConnection;
    };

    typedef websocket_connection<hc::ws::client_config> client_connection;
    typedef websocket_connection<hc::ws::server_config> server_connection;

}
}