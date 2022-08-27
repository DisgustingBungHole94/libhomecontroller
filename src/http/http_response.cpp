#include "homecontroller/http/http_response.h"

#include <llhttp.h>

namespace hc {
namespace http {

    void http_response::addHeader(const std::string& name, const std::string& value) {
        m_headers.insert(std::make_pair(name, value));
    }

    bool http_response::getHeader(const std::string& name, std::string& valueRef) {
        auto mit = m_headers.find(name);
        if (mit == m_headers.end()) {
            return false;
        }

        valueRef = mit->second;
        return true;
    }

    std::string http_response::toString() {
        std::string response = "HTTP/1.1 " + m_status + "\r\n";

        for (auto& x : m_headers) {
            response += x.first + ": " + x.second + "\r\n";
        }

        response += "\r\n" + m_body;

        return response;
    }

}
}