#pragma once

#include "homecontroller/api/device/packet_body.h"
#include "homecontroller/app/device_type.h"

namespace hc {
namespace api {
namespace device {
    
    class command_packet_body : public packet_body {
        public:
            command_packet_body() {}
            ~command_packet_body() {}

            bool parse(const std::string& bytes);
            std::string to_bytes() const;

            void set_device_type(device_type type) { m_device_type = type; }
            device_type get_device_type() { return m_device_type; }

            void set_command_id(uint16_t id) { m_command_id = id; }
            uint16_t get_command_id() { return m_command_id ;}

            static const int header_size = 6;

        private:
            device_type m_device_type;
            uint16_t m_command_id;

    };

}
}
}