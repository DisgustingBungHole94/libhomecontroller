#pragma once

#include "homecontroller/api/json/api_message.h"

namespace hc {
namespace api {
namespace json {

    class register_device_api_request : public api_message {
        public:
            
            register_device_api_request()
                : register_device_api_request("", "")
            {}

            register_device_api_request(const std::string& device_name, const std::string& device_type) 
                : m_device_name(device_name), m_device_type(device_type)
            {}

            ~register_device_api_request() {}

            std::string str() const;

            void set_device_name(const std::string& device_name) { m_device_name = device_name; }
            const std::string& get_device_name() const { return m_device_name; }

            void set_device_type(const std::string& device_type) { m_device_type = device_type; }
            const std::string& get_device_type() const { return m_device_type; }
    
        private:
            bool get_data(util::json_document& json_doc);

            std::string m_device_name;
            std::string m_device_type;
        
    };

}
}
}