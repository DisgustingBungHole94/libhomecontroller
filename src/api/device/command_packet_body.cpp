#include "homecontroller/api/device/command_packet_body.h"

#include "homecontroller/api/device/packet.h"
#include "homecontroller/app/device_type.h"
#include "homecontroller/util/bytes.h"

namespace hc {
namespace api {
namespace device {
    
    bool command_packet_body::parse(const std::string& bytes) {
        if (bytes.size() < packet::header_size + header_size) {
            m_error_msg = "body too small";
            return false;
        }

        m_device_type = device_from_id(util::bytes::bytes_to_uint32(bytes, packet::header_size));
        m_command_id = util::bytes::bytes_to_uint16(bytes, packet::header_size + 4);

        return true;
    }

    std::string command_packet_body::to_bytes() const {
        std::string bytes;

        bytes.reserve(header_size);
    }

}
}
}