#include "homecontroller/api/json/response/register_device_api_response.h"

#include "homecontroller/util/json_document.h"
#include "homecontroller/exception/exception.h"

namespace hc {
namespace api {
namespace json {

    std::string register_device_api_response::str() {
        std::string json_str =
            "{"
                "\"success\":true,"
                "\"device_id\":\"" + m_device_id + "\""
            "}";

        return json_str;
    }

    bool register_device_api_response::get_data(util::json_document& json_doc) {
        m_device_id = json_doc.get_string("device_id");
        
        return true;
    }

}
}
}