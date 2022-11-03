#pragma once

#include <string>

namespace hc {
namespace api {
namespace device {
    
    class packet_body {
        public:
            packet_body() {}
            ~packet_body() {}

            virtual bool parse(const std::string& bytes);
            virtual std::string to_bytes() const;

            const std::string& get_error_msg() { return m_error_msg; }

        protected:
            std::string m_error_msg;
    };

}
}
}