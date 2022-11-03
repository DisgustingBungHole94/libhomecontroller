#include "homecontroller/api/json/request/register_device_api_request.h"

#include "homecontroller/util/json_document.h"
#include "homecontroller/exception/exception.h"

namespace hc {
namespace api {
namespace json {

    std::string register_device_api_request::str() const {
        std::string json_str =
            "{"
                "\"device_name\":\"" + m_device_name + "\","
                "\"device_type\":\"" + m_device_type + "\""
            "}";

        return json_str;
    }

    bool register_device_api_request::get_data(util::json_document& json_doc) {
        m_device_name = json_doc.get_string("device_name");
        m_device_type = json_doc.get_string("device_type");

        return true;       
    }

}
}
}