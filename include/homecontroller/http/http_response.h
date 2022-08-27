#pragma once

#include <string>
#include <map>

namespace hc {
namespace http {
    
    class http_response {
        public:
            http_response()
                : http_response("", "")
            {}

            http_response(const std::string& status, const std::string& body)
                : http_response(status, body, {})
            {}

            http_response(const std::string& status, const std::string& body, const std::map<std::string, std::string>& headers)
                : m_status(status), m_body(body), m_headers(headers)
            {}

            ~http_response() {}

            void setStatus(const std::string& status) { m_status = status; }
            const std::string& getStatus() { return m_status; }

            void setBody(const std::string& body) { m_body = body; }
            const std::string& getBody() { return m_body; }

            void setShouldUpgrade(bool shouldUpgrade) { m_shouldUpgrade = shouldUpgrade; }
            bool shouldUpgrade() { return m_shouldUpgrade; }

            void setHeaders(const std::map<std::string, std::string>& headers) { m_headers = headers; }
            void addHeader(const std::string& name, const std::string& value);
            bool getHeader(const std::string& name, std::string& valueRef);

            std::string toString();

        private:
            std::string m_status;
            std::string m_body;

            bool m_shouldUpgrade;

            std::map<std::string, std::string> m_headers;
    };

}
}