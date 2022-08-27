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

            void setMethod(const std::string& method) { m_method = method; }
            const std::string& getMethod() { return m_method; }

            void setUrl(const std::string& url) { m_url = url; }
            const std::string& getUrl() { return m_url; }

            void setBody(const std::string& body) { m_body = body; }
            const std::string& getBody() { return m_body; }

            void setShouldUpgrade(bool shouldUpgrade) { m_shouldUpgrade = shouldUpgrade; }
            bool shouldUpgrade() { return m_shouldUpgrade; }

            void setHeaders(const std::map<std::string, std::string>& headers) { m_headers = headers; }
            void addHeader(const std::string& name, const std::string& value);
            bool getHeader(const std::string& name, std::string& valueRef);

            std::string toString();

        private:
            std::string m_method;
            std::string m_url;
            std::string m_body;

            bool m_shouldUpgrade;

            std::map<std::string, std::string> m_headers;
    };

}
}