#pragma once

#include "homecontroller/api/json/api_message.h"

namespace hc {
namespace api {
namespace json {
    
    struct login_api_request_data {
        std::string m_username;
        std::string m_password;
    };

    class login_api_request : public api_message {
        public:

            login_api_request() 
                : login_api_request("", "")
            {}

            login_api_request(const std::string& username, const std::string& password)
                : m_username(username), m_password(password)
            {}

            ~login_api_request() {}

            std::string str() const;

            void set_username(const std::string& username) { m_username = username; }
            const std::string& get_username() const { return m_username; }

            void set_password(const std::string& password) { m_password = password; }
            const std::string& get_password() const { return m_password; }

        private:
            bool get_data(util::json_document& json_doc);

            std::string m_username;
            std::string m_password;

    };

}
}
}