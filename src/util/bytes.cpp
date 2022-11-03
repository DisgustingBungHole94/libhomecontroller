#include "homecontroller/util/bytes.h"

namespace hc {
namespace util {
namespace bytes {

    uint32_t bytes_to_uint32(const std::string& bytes, int pos) {
        uint32_t val = 0x00;
        val |= (uint32_t)(bytes[pos + 0]) << 24;
        val |= (uint32_t)(bytes[pos + 1]) << 16;
        val |= (uint32_t)(bytes[pos + 2]) << 8;
        val |= (uint32_t)(bytes[pos + 3]);

        return val;
    }

    uint16_t bytes_to_uint16(const std::string& bytes, int pos) {
        uint16_t val = 0x00;
        val |= (uint32_t)(bytes[pos + 0]) << 8;
        val |= (uint32_t)(bytes[pos + 1]);

        return val;
    }

    std::string uint32_to_bytes(uint32_t val) {
        
    }
    
    std::string uint16_to_bytes(uint16_t val) {

    }

}
}
}