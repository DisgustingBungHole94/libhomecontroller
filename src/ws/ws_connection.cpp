#include "homecontroller/ws/ws_connection.h"

#include "homecontroller/exception/exception.h"

namespace hc {
namespace ws {
    
    template<class config>
    void websocket_connection<config>::upgrade(const std::string& upgrade_request) {
        m_ws_conn_ptr->read_all(upgrade_request.c_str(), upgrade_request.length());
    }

    template<class config>
    void websocket_connection<config>::start() {
        m_ws_conn_ptr->set_vector_write_handler(std::bind(&websocket_connection::on_vector_write, this, std::placeholders::_1, std::placeholders::_2));
        m_ws_conn_ptr->set_write_handler(std::bind(&websocket_connection::on_write, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        m_ws_conn_ptr->set_open_handler(std::bind(&websocket_connection::on_open, this, std::placeholders::_1));
        m_ws_conn_ptr->set_fail_handler(std::bind(&websocket_connection::on_fail, this, std::placeholders::_1));
        m_ws_conn_ptr->set_close_handler(std::bind(&websocket_connection::on_close, this, std::placeholders::_1));
        m_ws_conn_ptr->set_message_handler(std::bind(&websocket_connection::_on_message, this, std::placeholders::_1, std::placeholders::_2));
    
        m_ws_conn_ptr->start();
    }

    template<class config>
    void websocket_connection<config>::stop() {
        if (!m_finished) {
            close("server closed");
        }
    }

    template<class config>
    void websocket_connection<config>::on_data() {
        try {
            std::string data = get_connection()->recv();
            m_ws_conn_ptr->read_all(data.c_str(), data.length());
        } catch(exception& e) {
            if (!m_finished) {
                m_logger.err("recv error: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
                m_finished = false;
            }
        }
    }

    template<class config>
    void websocket_connection<config>::send(const std::string& msg) {
        try {
            m_ws_conn_ptr->send(msg, websocketpp::frame::opcode::BINARY);
        } catch(std::exception& e) {
            m_logger.err("failed to send message: " + std::string(e.what()));
        }
    }

    template<class config>
    void websocket_connection<config>::close(const std::string& reason) {
        try {
            m_ws_conn_ptr->close(websocketpp::close::status::going_away, reason);
        } catch(std::exception& e) {
            m_logger.err("failed to close client: " + std::string(e.what()));
        }
    }

    template<class config>
    std::error_code websocket_connection<config>::on_vector_write(websocketpp::connection_hdl hdl, std::vector<websocketpp::transport::buffer> const& bufs) {
        if (m_finished) {
            return std::make_error_code(std::errc::connection_aborted);
        }

        std::string data;
        
        for (int i = 0; i < bufs.size(); i++) {
            data.append(bufs[i].buf, bufs[i].len);
        }

        try {
            get_connection()->send(data);
        } catch(exception& e) {
            m_logger.err("send error: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
            m_finished = true;

            return std::make_error_code(std::errc::io_error);
        }

        return std::error_code();
    }

    template<class config>
    std::error_code websocket_connection<config>::on_write(websocketpp::connection_hdl hdl, const char* data, std::size_t len) {
        if (m_finished) {
            return std::make_error_code(std::errc::connection_aborted);
        }

        try {
            get_connection()->send(std::string(data, len));
        } catch(exception& e) {
            m_logger.err("send error: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");

            m_finished = true;
            return std::make_error_code(std::errc::io_error);
        }

        return std::error_code();
    }

    template<class config>
    void websocket_connection<config>::on_open(websocketpp::connection_hdl hdl) {
        m_logger.dbg("client session started");
    }

    template<class config>
    void websocket_connection<config>::on_fail(websocketpp::connection_hdl hdl) {
        m_logger.err("failed to start client session");
        m_finished = true;
    }

    template<class config>
    void websocket_connection<config>::on_close(websocketpp::connection_hdl hdl) {
        m_logger.dbg("client session finished");
        m_finished = true;
    }

    template<class config>
    void websocket_connection<config>::_on_message(websocketpp::connection_hdl hdl, server::message_ptr msg) {
        if (msg->get_opcode() != websocketpp::frame::opcode::binary) return;

        on_message(msg->get_payload());
    }

    template<class config>
    void websocket_connection<config>::on_message(const std::string& msg) {

    }

    template<class config>
    net::ssl::connection_ptr websocket_connection<config>::get_connection() {
        if (m_tls_conn_hdl.expired()) {
            throw exception("bad connection", "hc::ws::websocket_connection::get_connection");
        }

        return m_tls_conn_hdl.lock();
    }

    template class websocket_connection<client_config>;
    template class websocket_connection<server_config>;

}
}