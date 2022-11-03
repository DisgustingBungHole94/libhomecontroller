#pragma once

#include "homecontroller/api/json/api_message.h"

namespace hc {
namespace api {
namespace json {

    class login_api_response : public api_message {
        public:

            login_api_response()
                : login_api_response("")
            {}

            login_api_response(const std::string& token) 
                : api_message(true), m_token(token) 
            {}

            ~login_api_response() {}

            std::string str();

            void set_token(const std::string& token) { m_token = token; }
            const std::string& get_token() { return m_token; }

        private:
            bool get_data(util::json_document& json_doc);

            std::string m_token;
    };

}
}
}