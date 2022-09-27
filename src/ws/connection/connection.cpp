#include "homecontroller/ws/connection/connection.h"

#include "homecontroller/exception/exception.h"

namespace hc {
namespace ws {

    template<class config>
    void connection<config>::init() {
        m_ws_conn_ptr->set_vector_write_handler(std::bind(&connection::on_vector_write, this, std::placeholders::_1, std::placeholders::_2));
        m_ws_conn_ptr->set_write_handler(std::bind(&connection::on_write, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        m_ws_conn_ptr->set_open_handler(std::bind(&connection::on_open, this, std::placeholders::_1));
        m_ws_conn_ptr->set_fail_handler(std::bind(&connection::on_fail, this, std::placeholders::_1));
        m_ws_conn_ptr->set_close_handler(std::bind(&connection::on_close, this, std::placeholders::_1));
        m_ws_conn_ptr->set_message_handler(std::bind(&connection::_on_message, this, std::placeholders::_1, std::placeholders::_2));
    
        m_ws_conn_ptr->start();
    }

    template<class config>
    void connection<config>::start() {}

    template<class config>
    void connection<config>::stop() {
        if (!m_finished) {
            close("connection closed");
        }
    }

    template<class config>
    void connection<config>::read_data(const std::string& data) {
        m_ws_conn_ptr->read_all(data.c_str(), data.length());
    }

    template<class config>
    void connection<config>::send(const std::string& msg) {
        try {
            m_ws_conn_ptr->send(msg, websocketpp::frame::opcode::TEXT);
        } catch(std::exception& e) {
            m_logger.err("failed to send message: " + std::string(e.what()));
        }
    }

    template<class config>
    void connection<config>::close(const std::string& reason) {
        try {
            m_ws_conn_ptr->close(websocketpp::close::status::going_away, reason);
        } catch(std::exception& e) {
            m_logger.err("failed to close client: " + std::string(e.what()));
        }
    }

    template<class config>
    std::error_code connection<config>::on_vector_write(websocketpp::connection_hdl hdl, std::vector<websocketpp::transport::buffer> const& bufs) {
        if (m_finished) {
            return std::make_error_code(std::errc::connection_aborted);
        }

        std::string data;
        
        for (int i = 0; i < bufs.size(); i++) {
            data.append(bufs[i].buf, bufs[i].len);
        }

        try {
            net::ssl::connection::conn_from_hdl(m_tls_conn_hdl)->send(data);
        } catch(exception& e) {
            m_logger.err("send error: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
            m_finished = true;

            return std::make_error_code(std::errc::io_error);
        }

        return std::error_code();
    }

    template<class config>
    std::error_code connection<config>::on_write(websocketpp::connection_hdl hdl, const char* data, std::size_t len) {        
        if (m_finished) {
            return std::make_error_code(std::errc::connection_aborted);
        }

        try {
            net::ssl::connection::conn_from_hdl(m_tls_conn_hdl)->send(std::string(data, len));
        } catch(exception& e) {
            m_logger.err("send error: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");

            m_finished = true;
            return std::make_error_code(std::errc::io_error);
        }

        return std::error_code();
    }

    template<class config>
    void connection<config>::on_open(websocketpp::connection_hdl hdl) {
        m_logger.dbg("client session started");
    }

    template<class config>
    void connection<config>::on_fail(websocketpp::connection_hdl hdl) {
        m_logger.err("failed to start client session");
        m_finished = true;
    }

    template<class config>
    void connection<config>::on_close(websocketpp::connection_hdl hdl) {
        m_logger.dbg("client session finished");
        m_finished = true;
    }

    template<class config>
    void connection<config>::_on_message(websocketpp::connection_hdl hdl, server::message_ptr msg) {
        //if (msg->get_opcode() != websocketpp::frame::opcode::binary) return;

        m_message_callback(msg->get_payload());
    }

    template class connection<client_config>;
    template class connection<server_config>;

}
}