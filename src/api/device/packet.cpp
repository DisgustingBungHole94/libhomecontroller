#include "homecontroller/api/device/packet.h"

#include "homecontroller/util/bytes.h"

namespace hc {
namespace api {
namespace device {
    
    bool packet::parse_header(const std::string& bytes) {
        if (bytes.size() < header_size) {
            m_error_msg = "header too small";
            return false;
        }

        m_length = util::bytes::bytes_to_uint32(bytes, 0);
        m_id = util::bytes::bytes_to_uint32(bytes, 4);
    }

    std::string packet::to_bytes(const packet_body& body) const {
        std::string header_bytes;
        
        header_bytes.reserve(header_size);

        header_bytes.append(util::bytes::uint32_to_bytes(m_length));
        header_bytes.append(util::bytes::uint32_to_bytes(m_id));

        header_bytes.append(body.to_bytes());

        return header_bytes;
    }

    packet_type packet::get_type() {
        packet_type type;

        switch(m_id) {
            case 0x01:
                type = packet_type::HANDSHAKE;
                break;
            case 0x02:
                type = packet_type::HANDSHAKE_RESPONSE;
                break;
            case 0x03:
                type = packet_type::COMMAND;
                break;
            case 0x04:
                type = packet_type::COMMAND_RESPONSE;
                break;
            default:
                type = packet_type::UNKNOWN;
                break;
        }

        return type;
    }

}
}
}