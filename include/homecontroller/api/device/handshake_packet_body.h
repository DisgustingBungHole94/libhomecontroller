#pragma once

#include "homecontroller/api/device/packet_body.h"

namespace hc {
namespace api {
namespace device {
    
    class handshake_packet_body : public packet_body {
        public:
            handshake_packet_body() {}
            ~handshake_packet_body() {}

            bool parse(const std::string& bytes);
            std::string to_bytes() const;

            void set_protocol_version(uint16_t protocol_version) { m_protocol_version = protocol_version; }
            uint16_t get_protocol_version() { return m_protocol_version; }

            void set_session_token(const std::string& session_token) { m_session_token = session_token; }
            const std::string& get_session_token() { return m_session_token; }

            void set_device_id(const std::string& device_id) { m_device_id = device_id; }
            const std::string& get_device_id() { return m_device_id; }

            static const int size = 66;

        private:
            uint16_t m_protocol_version;

            std::string m_session_token;
            std::string m_device_id;
    };

}
}
}