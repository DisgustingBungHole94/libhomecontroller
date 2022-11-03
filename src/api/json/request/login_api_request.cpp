#include "homecontroller/api/json/request/login_api_request.h"

#include "homecontroller/util/json_document.h"
#include "homecontroller/exception/exception.h"

#include <iostream>

namespace hc {
namespace api {
namespace json {

    std::string login_api_request::str() const {
        std::string json_str = 
            "{"
                "\"username\":\"" + m_username + "\","
                "\"password\":\"" + m_password + "\""
            "}";
        
        return json_str;
    }

    bool login_api_request::get_data(util::json_document& json_doc) {
        m_username = json_doc.get_string("username");
        m_password = json_doc.get_string("password");

        return true;
    }

}
}
}