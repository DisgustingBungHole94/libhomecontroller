#include "homecontroller/http/http_request.h"

namespace hc {
namespace http {
    void http_request::add_header(const std::string& name, const std::string& value) {
        m_headers.insert(std::make_pair(name, value));
    }

    bool http_request::get_header(const std::string& name, std::string& value_ref) const {
        auto mit = m_headers.find(name);
        if (mit == m_headers.end()) {
            return false;
        }

        value_ref = mit->second;
        return true;
    }

    std::string http_request::str() const {
        std::string request = m_method + " " + m_url + " HTTP/1.1\r\n";

        for (auto& x : m_headers) {
            request += x.first + ": " + x.second + "\r\n";
        }

        request += "\r\n" + m_body;

        return request;
    }
}
}