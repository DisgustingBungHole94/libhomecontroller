#pragma once

#include "homecontroller/http/http_response.h"

namespace hc {
namespace http {
namespace common {
    
    extern http_response ok();

    // generic error
    extern http_response error(const std::string& method, const std::string& error_msg, int error_code);
    
    // protocol errors (0-4900);
    extern http_response server_error();
    extern http_response bad_http_request();
    extern http_response need_auth();

    // module errors ((-5000)-(-20000))
    extern http_response not_found();
    extern http_response bad_json();
    extern http_response method_not_allowed();

    // login errors ((-25000)-(30000))
    extern http_response user_not_found();
    extern http_response incorrect_password();

}
}
}