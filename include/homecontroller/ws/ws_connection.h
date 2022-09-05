#pragma once

#include "homecontroller/ws/ws_config.h"
#include "homecontroller/net/ssl/tls_connection.h"
#include "homecontroller/util/logger.h"

#include <functional>
#include <vector>

namespace hc {
namespace ws {

    template<typename config>
    class websocket_connection {
        public:
            typedef std::shared_ptr<websocketpp::connection<config>> ws_connection_ptr;

            websocket_connection(ws_connection_ptr ws_conn_ptr, net::ssl::connection_hdl tls_conn_hdl) 
                : m_logger("websocket_connection"), m_ws_conn_ptr(ws_conn_ptr), m_tls_conn_hdl(tls_conn_hdl)
            {}

            ~websocket_connection() {}

            void upgrade(const std::string& upgrade_request);

            void start();
            void stop();

            void on_data();

            void send(const std::string& msg);
            void close(const std::string& reason);

            virtual void on_message(const std::string& msg);

        private:
            std::error_code on_vector_write(websocketpp::connection_hdl hdl, std::vector<websocketpp::transport::buffer> const& bufs);
            std::error_code on_write(websocketpp::connection_hdl hdl, const char* data, std::size_t len);

            void on_open(websocketpp::connection_hdl hdl);
            void on_fail(websocketpp::connection_hdl hdl);
            void on_close(websocketpp::connection_hdl hdl);
            void _on_message(websocketpp::connection_hdl hdl, server::message_ptr msg);

            net::ssl::connection_ptr get_connection();

            util::logger m_logger;

            ws_connection_ptr m_ws_conn_ptr;
            net::ssl::connection_hdl m_tls_conn_hdl;

            bool m_finished;
    };

    typedef websocket_connection<client_config> client_connection;
    typedef websocket_connection<server_config> server_connection;

}
}