#pragma once

#include "homecontroller/api/device/packet_body.h"

namespace hc {
namespace api {
namespace device {
    
    class command_response_packet_body : public packet_body {
        public:
            command_response_packet_body() {}
            ~command_response_packet_body() {}

            bool parse(const std::string& bytes);
            std::string to_bytes() const;

            void set_success(bool success) { m_success = success; }
            bool get_success() { return m_success; }

        private:
            bool m_success;
    };

}
}
}