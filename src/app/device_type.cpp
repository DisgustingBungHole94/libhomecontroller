#include "homecontroller/app/device_type.h"

#include <unordered_map>

namespace hc {

    device_type device_from_id(uint32_t device_id) {
        device_type type;
        
        switch(device_id) {
            case 0x01:
                type = device_type::LIGHT_STRIP;
                break;
            default:
                type = device_type::UNKNOWN;
                break;
        }

        return type;
    }

    device_type device_from_string(const std::string& device_str) {
        device_type type;

        static const std::unordered_map<std::string, device_type> device_types = {
            { "light_strip", device_type::LIGHT_STRIP }
        };

        auto mit = device_types.find(device_str);
        if (mit == device_types.end()) {
            type = device_type::UNKNOWN;
        } else {
            type = mit->second;
        }

        return type;
    }

    uint8_t id_from_device(device_type type) {
        uint8_t id;

        switch(type) {
            case device_type::LIGHT_STRIP:
                id = 0x01;
                break;
            default:
                id = 0x00;
                break;
        }

        return id;
    }

    std::string str_from_device(device_type type) {
        std::string str;

        switch(type) {
            case device_type::LIGHT_STRIP:
                str = "light_strip";
                break;
            default:
                str = "unknown";
                break;
        }

        return str;
    }

}