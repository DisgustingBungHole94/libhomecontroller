#pragma once

#include "homecontroller/api/device/packet_body.h"

#include <string>

namespace hc {
namespace api {
namespace device {
    
    enum class packet_type {
        UNKNOWN,
        HANDSHAKE,
        HANDSHAKE_RESPONSE,
        COMMAND,
        COMMAND_RESPONSE
    };

    class packet {
        public:
            packet() 
                :  m_error_msg(""), m_length(0), m_id(0)
            {}

            ~packet() {}

            bool parse_header(const std::string& bytes);
            std::string to_bytes(const packet_body& body) const;

            const std::string& get_error_msg() { return m_error_msg; }

            void set_length(uint32_t length) { m_length = length; }
            uint32_t get_length() { return m_length; }

            packet_type get_type();

            static const int header_size = 8;

        private:
            std::string m_error_msg;

            uint32_t m_length;
            uint32_t m_id;
    };

}
}
}