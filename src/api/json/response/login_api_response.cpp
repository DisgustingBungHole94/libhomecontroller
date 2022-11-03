#include "homecontroller/api/json/response/login_api_response.h"

#include "homecontroller/util/json_document.h"
#include "homecontroller/exception/exception.h"

#include <iostream>

namespace hc {
namespace api {
namespace json {

    std::string login_api_response::str() {
        std::string json_str = 
            "{"
                "\"success\":true,"
                "\"token\":\"" + m_token + "\""
            "}";

        return json_str;
    }

    bool login_api_response::get_data(util::json_document& json_doc) {
        m_token = json_doc.get_string("token");
        
        return true;
    }

}
}
}