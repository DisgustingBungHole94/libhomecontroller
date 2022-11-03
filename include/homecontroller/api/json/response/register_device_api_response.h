#pragma once

#include "homecontroller/api/json/api_message.h"

namespace hc {
namespace api {
namespace json {

    class register_device_api_response : public api_message {
        public:

            register_device_api_response() 
                : register_device_api_response("")
            {}

            register_device_api_response(const std::string& device_id)
                : api_message(true), m_device_id(device_id)
            {}

            ~register_device_api_response() {}

            std::string str();

            void set_device_id(const std::string& device_id) { m_device_id = device_id; }
            const std::string& get_device_id() { return m_device_id; }

        private:
            bool get_data(util::json_document& json_doc);

            std::string m_device_id;
    };

}
}
}