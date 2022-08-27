#include "homecontroller/ws/ws_connection.h"

#include "homecontroller/exception/exception.h"

namespace hc {
namespace ws {
    
    template<class config>
    void websocket_connection<config>::upgrade(const std::string& upgradeRequest) {
        m_wsConnection->read_all(upgradeRequest.c_str(), upgradeRequest.length());
    }

    template<class config>
    void websocket_connection<config>::start() {
        m_wsConnection->set_vector_write_handler(std::bind(&websocket_connection::onVectorWrite, this, std::placeholders::_1, std::placeholders::_2));
        m_wsConnection->set_write_handler(std::bind(&websocket_connection::onWrite, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        m_wsConnection->set_open_handler(std::bind(&websocket_connection::onOpen, this, std::placeholders::_1));
        m_wsConnection->set_fail_handler(std::bind(&websocket_connection::onFail, this, std::placeholders::_1));
        m_wsConnection->set_close_handler(std::bind(&websocket_connection::onClose, this, std::placeholders::_1));
        m_wsConnection->set_message_handler(std::bind(&websocket_connection::onMessage, this, std::placeholders::_1, std::placeholders::_2));
    
        m_wsConnection->start();
    }

    template<class config>
    void websocket_connection<config>::stop() {
        if (!m_finished) {
            close("server closed");
        }
    }

    template<class config>
    void websocket_connection<config>::onReady() {
        try {
            std::string data = m_conn->recv();
            m_wsConnection->read_all(data.c_str(), data.length());
        } catch(hc::exception& e) {
            if (!m_finished) {
                m_logger.err("recv error: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
                m_finished = false;
            }
        }
    }

    template<class config>
    void websocket_connection<config>::send(const std::vector<uint8_t>& msg) {
        try {
            m_wsConnection->send(&msg[0], msg.size(), websocketpp::frame::opcode::binary);
        } catch(std::exception& e) {
            m_logger.err("failed to send message: " + std::string(e.what()));
        }
    }

    template<class config>
    void websocket_connection<config>::close(const std::string& reason) {
        try {
            m_wsConnection->close(websocketpp::close::status::going_away, reason);
        } catch(std::exception& e) {
            m_logger.err("failed to close client: " + std::string(e.what()));
        }
    }

    template<class config>
    std::error_code websocket_connection<config>::onVectorWrite(websocketpp::connection_hdl hdl, std::vector<websocketpp::transport::buffer> const& bufs) {
        if (m_finished) {
            return std::make_error_code(std::errc::connection_aborted);
        }

        std::string data;
        
        for (int i = 0; i < bufs.size(); i++) {
            data.append(bufs[i].buf, bufs[i].len);
        }

        try {
            m_conn->send(data);
        } catch(hc::exception& e) {
            m_logger.err("send error: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
            m_finished = true;

            return std::make_error_code(std::errc::io_error);
        }

        return std::error_code();
    }

    template<class config>
    std::error_code websocket_connection<config>::onWrite(websocketpp::connection_hdl hdl, const char* data, std::size_t len) {
        if (m_finished) {
            return std::make_error_code(std::errc::connection_aborted);
        }

        try {
            m_conn->send(std::string(data, len));
        } catch(hc::exception& e) {
            m_logger.err("send error: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");

            m_finished = true;
            return std::make_error_code(std::errc::io_error);
        }

        return std::error_code();
    }

    template<class config>
    void websocket_connection<config>::onOpen(websocketpp::connection_hdl hdl) {
        m_logger.dbg("client session started");
    }

    template<class config>
    void websocket_connection<config>::onFail(websocketpp::connection_hdl hdl) {
        m_logger.err("failed to start client session");
        m_finished = true;
    }

    template<class config>
    void websocket_connection<config>::onClose(websocketpp::connection_hdl hdl) {
        m_logger.dbg("client session finished");
        m_finished = true;
    }

    template<class config>
    void websocket_connection<config>::onMessage(websocketpp::connection_hdl hdl, hc::ws::server::message_ptr msg) {
        if (msg->get_opcode() != websocketpp::frame::opcode::binary) return;

        std::string messageStr = msg->get_payload();

        m_messageCallback(std::vector<uint8_t>(messageStr.cbegin(), messageStr.cend()));
    }

    template class websocket_connection<hc::ws::client_config>;
    template class websocket_connection<hc::ws::server_config>;

}
}