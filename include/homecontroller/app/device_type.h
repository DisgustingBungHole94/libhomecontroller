#pragma once

#include <string>

namespace hc {

    enum class device_type {
        UNKNOWN,
        LIGHT_STRIP
    };

    extern device_type device_from_id(uint32_t device_id);
    extern device_type device_from_str(const std::string& device_str);

    extern uint8_t id_from_device(device_type type);
    extern std::string str_from_device(device_type type);

}