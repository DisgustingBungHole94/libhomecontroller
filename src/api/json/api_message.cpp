#include "homecontroller/api/json/api_message.h"

#include "homecontroller/api/json/request/login_api_request.h"
#include "homecontroller/api/json/response/login_api_response.h"

#include "homecontroller/api/json/request/register_device_api_request.h"
#include "homecontroller/api/json/response/register_device_api_response.h"

#include "homecontroller/exception/exception.h"

#include <iostream>

namespace hc {
namespace api {
namespace json {

    bool api_message::parse(const std::string& data) {
        try {
            util::json_document json_doc;
            json_doc.parse(data);

            if (m_check_success) {
                if (!json_doc.get_bool("success")) {
                    m_error_msg = json_doc.get_string("error_msg");
                    m_error_code = json_doc.get_int("error_code");

                    return false;
                }
            }

            if (!get_data(json_doc)) {
                return false;
            }
        } catch(hc::exception& e) {
            m_error_msg = e.what();
            m_error_code = 1;

            return false;
        }

        return true;
    }

    std::string api_message::str() const {
        return "";
    }

    bool api_message::get_data(util::json_document& json_doc) {
        return false;
    }

}
}
}