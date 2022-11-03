#include "homecontroller/api/device/packet_body.h"

namespace hc {
namespace api {
namespace device {
    
    bool packet_body::parse(const std::string& bytes) {
        return false;
    }

    std::string packet_body::to_bytes() const {
        return "";
    }

}
}
}