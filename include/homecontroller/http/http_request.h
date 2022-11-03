#pragma once

#include <string>
#include <map>

namespace hc {
namespace http {

    class http_request {
        public:
            http_request() 
                : http_request("", "", "")
            {}

            http_request(const std::string& method, const std::string& url, const std::string& body)
                : http_request(method, url, body, {})
            {}

            http_request(const std::string& method, const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers)
                : m_method(method), m_url(url), m_body(body), m_headers(headers)
            {}
            
            ~http_request() {}

            void set_method(const std::string& method) { m_method = method; }
            const std::string& get_method() const { return m_method; }

            void set_url(const std::string& url) { m_url = url; }
            const std::string& get_url() const { return m_url; }

            void set_body(const std::string& body) { m_body = body; }
            const std::string& get_body() const { return m_body; }

            void set_should_upgrade(bool should_upgrade) { m_should_upgrade = should_upgrade; }
            bool should_upgrade() const { return m_should_upgrade; }

            void set_headers(const std::map<std::string, std::string>& headers) { m_headers = headers; }
            void add_header(const std::string& name, const std::string& value);
            bool get_header(const std::string& name, std::string& value_ref) const;

            std::string str() const;

        private:
            std::string m_method;
            std::string m_url;
            std::string m_body;

            bool m_should_upgrade;

            std::map<std::string, std::string> m_headers;
    };

}
}