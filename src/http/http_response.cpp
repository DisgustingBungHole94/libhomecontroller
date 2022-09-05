#include "homecontroller/http/http_response.h"

#include <llhttp.h>

namespace hc {
namespace http {

    void http_response::add_header(const std::string& name, const std::string& value) {
        m_headers.insert(std::make_pair(name, value));
    }

    bool http_response::get_header(const std::string& name, std::string& value_ref) {
        auto mit = m_headers.find(name);
        if (mit == m_headers.end()) {
            return false;
        }

        value_ref = mit->second;
        return true;
    }

    std::string http_response::str() {
        std::string response = "HTTP/1.1 " + m_status + "\r\n";

        for (auto& x : m_headers) {
            response += x.first + ": " + x.second + "\r\n";
        }

        response += "\r\n" + m_body;

        return response;
    }

}
}