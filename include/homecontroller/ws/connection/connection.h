#pragma once

#include "homecontroller/ws/ws_config.h"
#include "homecontroller/net/ssl/connection/connection.h"
#include "homecontroller/util/logger.h"

#include <functional>
#include <vector>

namespace hc {
namespace ws {

    template<typename config>
    class connection {
        public:
            typedef std::shared_ptr<websocketpp::connection<config>> ws_conn_ptr;
            typedef std::function<void(const std::string&)> message_callback;

            connection(ws_conn_ptr ws_conn_ptr, net::ssl::conn_hdl tls_conn_hdl) 
                : m_logger("ws::connection"), m_ws_conn_ptr(ws_conn_ptr), m_tls_conn_hdl(tls_conn_hdl), m_finished(false)
            {}

            ~connection() {}

            virtual void start();
            void stop();

            void read_data(const std::string& data);

            void send(const std::string& msg);
            void close(const std::string& reason);

            void set_message_callback(message_callback callback) { m_message_callback = callback; }

            bool finished() { return m_finished; }

        private:
            std::error_code on_vector_write(websocketpp::connection_hdl hdl, std::vector<websocketpp::transport::buffer> const& bufs);
            std::error_code on_write(websocketpp::connection_hdl hdl, const char* data, std::size_t len);

            void on_open(websocketpp::connection_hdl hdl);
            void on_fail(websocketpp::connection_hdl hdl);
            void on_close(websocketpp::connection_hdl hdl);
            void _on_message(websocketpp::connection_hdl hdl, server::message_ptr msg);

            util::logger m_logger;

            ws_conn_ptr m_ws_conn_ptr;
            net::ssl::conn_hdl m_tls_conn_hdl;

            message_callback m_message_callback;

            bool m_finished;

        protected:
            void init();
    };

}
}