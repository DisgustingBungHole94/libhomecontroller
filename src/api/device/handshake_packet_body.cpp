#include "homecontroller/api/device/handshake_packet_body.h"

#include "homecontroller/api/device/packet.h"
#include "homecontroller/util/bytes.h"

namespace hc {
namespace api {
namespace device {
    
    bool handshake_packet_body::parse(const std::string& bytes) {
        if (bytes.size() < packet::header_size + size) {
            m_error_msg = "body too small";
            return false;
        }

        m_protocol_version = util::bytes::bytes_to_uint16(bytes, packet::header_size);

        m_session_token = bytes.substr(packet::header_size + 2, packet::header_size + 2 + 32);
        m_device_id = bytes.substr(packet::header_size + 2 + 32, packet::header_size + 2 + 32 + 32);

        return true;
    }

    std::string handshake_packet_body::to_bytes() const {
        std::string bytes;

        bytes.reserve(size);

        bytes.append(util::bytes::uint16_to_bytes(m_protocol_version));
        
        bytes.append(m_session_token);
        bytes.append(m_device_id);

        return bytes;
    }

}
}
}