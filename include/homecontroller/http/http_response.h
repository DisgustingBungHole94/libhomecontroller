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

            void set_status(const std::string& status) { m_status = status; }
            const std::string& get_status() const { return m_status; }

            void set_body(const std::string& body) { m_body = body; }
            const std::string& get_body() const { return m_body; }

            void set_should_upgrade(bool should_upgrade) { m_should_upgrade = should_upgrade; }
            bool should_upgrade() const { return m_should_upgrade; }

            void set_headers(const std::map<std::string, std::string>& headers) { m_headers = headers; }
            void add_header(const std::string& name, const std::string& value);
            bool get_header(const std::string& name, std::string& value_ref) const;

            std::string str() const;

        private:
            std::string m_status;
            std::string m_body;

            bool m_should_upgrade;

            std::map<std::string, std::string> m_headers;
    };

}
}