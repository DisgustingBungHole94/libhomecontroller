#pragma once

#include "homecontroller/api/device/packet_body.h"

namespace hc {
namespace api {
namespace device {
    
    enum class handshake_response_code {
        UNKNOWN,
        SUCCESS,
        BAD_SESSION_TOKEN,
        BAD_DEVICE_ID
    };

    class handshake_response_packet_body : public packet_body {
        public:
            handshake_response_packet_body() 
                : m_response_code(handshake_response_code::UNKNOWN)
            {}

            ~handshake_response_packet_body() {}

            bool parse(const std::string& bytes);
            std::string to_bytes() const;

            void set_response_code(handshake_response_code response_code) { m_response_code = response_code; }
            handshake_response_code get_response_code() { return m_response_code; }

            static const int size = 1;

        private:
            handshake_response_code m_response_code;
    };

}
}
}