#pragma once

#include "homecontroller/util/json_document.h"

#include <string>
#include <iostream>

namespace hc {
namespace api {
namespace json {
    
    class api_message {
        public:
            api_message() 
                : api_message(false)    
            {}

            api_message(bool check_success)
                : m_check_success(check_success), m_error_msg(""), m_error_code(0)
            {}
            
            ~api_message() {}

            bool parse(const std::string& data);

            virtual std::string str() const;

            const std::string& get_error_msg() const { return m_error_msg; }
            int get_error_code() const { return m_error_code; }

        protected:
            virtual bool get_data(util::json_document& json_doc); 

            bool m_check_success;

            std::string m_error_msg;
            int m_error_code;
    };

}
}
}