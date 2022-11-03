#include "homecontroller/api/device/handshake_response_packet_body.h"

#include "homecontroller/api/device/packet.h"
#include "homecontroller/util/bytes.h"
#include "homecontroller/exception/exception.h"

namespace hc {
namespace api {
namespace device {

    bool handshake_response_packet_body::parse(const std::string& bytes) {
        if (bytes.size() < packet::header_size + size) {
            m_error_msg = "body too small";
            return false;
        }

        switch(bytes[0]) {
            case 0x00:
                m_response_code = handshake_response_code::SUCCESS;
                break;
            case 0x01:
                m_response_code = handshake_response_code::BAD_SESSION_TOKEN;
                break;
            case 0x02:
                m_response_code = handshake_response_code::BAD_DEVICE_ID;
                break;
            default:
                m_response_code = handshake_response_code::UNKNOWN;
                break;
        }

        if (m_response_code == handshake_response_code::UNKNOWN) {
            m_error_msg = "unknown response code";
            return false;
        }

        return true;
    }

    std::string handshake_response_packet_body::to_bytes() const {
        std::string bytes;

        bytes.reserve(size);

        uint8_t response_code;

        switch(m_response_code) {
            case handshake_response_code::SUCCESS:
                response_code = 0x00;
                break;
            case handshake_response_code::BAD_SESSION_TOKEN:
                response_code = 0x01;
                break;
            case handshake_response_code::BAD_DEVICE_ID:
                response_code = 0x02;
                break;
            default:
                throw exception("unknown response code", "hc::api::device::handshake_response_packet_body::to_bytes");
                break;
        }

        bytes.append(std::to_string(response_code));

        return bytes;
    }

}
}
}