#pragma once

#include <string>

namespace hc {
namespace util {
namespace bytes {
    
    extern uint32_t bytes_to_uint32(const std::string& bytes, int pos);
    extern uint16_t bytes_to_uint16(const std::string& bytes, int pos);

    extern std::string uint32_to_bytes(uint32_t val);
    extern std::string uint16_to_bytes(uint16_t val);

}
}
}