#include "homecontroller/http/common_responses.h"

namespace hc {
namespace http {
namespace common {

    http_response ok() {
        std::string json_str =
            "{"
                "\"success\":true"
            "}";
        
        return http_response("200 OK", json_str);
    }

    // generic error
    http_response error(const std::string& method, const std::string& error_msg, int error_code) {
        std::string json_str = 
            "{"
                "\"success\":false,"
                "\"error_msg\":\"" + error_msg + "\","
                "\"error_code\":" + std::to_string(error_code) + ""
            "}";

        return http_response(method, json_str);
    }

    // protocol errors (0-4900);
    http_response server_error() {
        return error("500 Internal Server Error", "fatal error", 100);
    }

    http_response bad_http_request() {
        return error("400 Bad Request", "bad http request", 200);
    }

    http_response need_auth() {
        return error("401 Unauthorized", "authentication needed", 300);
    } 

    // module errors ((-5000)-(-20000))
    http_response not_found() {
        return error("404 Not Found", "module not found", -5000);
    }

    http_response bad_json() {
        return error("400 Bad Request", "bad json", -15000);
    }

    http_response method_not_allowed() {
        return error("405 Method Not Allowed", "http method not allowed", -20000);
    }

    // login errors ((-25000)-(30000))
    http_response user_not_found() {
        return error("400 Bad Request", "user not found", -25000);
    }

    http_response incorrect_password() {
        return error("400 Bad Request", "incorrect password", -30000);
    }

}
}
}