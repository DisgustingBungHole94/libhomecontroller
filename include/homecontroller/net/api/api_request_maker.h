#pragma once

#include "homecontroller/net/ssl/tls_client.h"
#include "homecontroller/http/http_parser.h"

#include "homecontroller/api/json/api_message.h"
#include "homecontroller/api/json/response/login_api_response.h"
#include "homecontroller/api/json/response/register_device_api_response.h"

namespace hc {
namespace net {
namespace api {
    
    class api_request_maker {
        public:
            api_request_maker() 
                : m_connected(false), m_logged_in(false)
            {}

            ~api_request_maker() {}

            void connect();
            void disconnect();

            void make_request(const std::string& method, const std::string& url, const hc::api::json::api_message& request, hc::api::json::api_message& response_ref);

            hc::api::json::login_api_response login(const std::string& username, const std::string& password);
            hc::api::json::register_device_api_response register_device(const std::string& device_name, const std::string& device_type);

            ssl::client_conn_ptr get_conn_ptr() { return m_conn_ptr; }

        private:
            void add_headers(hc::http::http_request& request_ref);

            ssl::client_conn_ptr m_conn_ptr;

            http::http_parser m_http_parser;

            bool m_connected;
            bool m_logged_in;

            std::string m_token;
     };

}
}
}