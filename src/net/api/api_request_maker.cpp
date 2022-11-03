#include "homecontroller/net/api/api_request_maker.h"

#include "homecontroller/exception/exception.h"
#include "homecontroller/net/endpoints.h"
#include "homecontroller/app/info.h"

#include "homecontroller/api/json/request/login_api_request.h"
#include "homecontroller/api/json/request/register_device_api_request.h"

#include <iostream>

namespace hc {
namespace net {
namespace api {

    void api_request_maker::connect() {
        if (!m_connected) {
            ssl::tls_client client;
            client.init();

            m_conn_ptr = client.connect(endpoints::API_HOST, net::endpoints::API_PORT);
            m_connected = true;

            m_http_parser.init(http::http_parser::parser_type::RESPONSE);
        }
    }

    void api_request_maker::disconnect() {
        if (m_connected) {
            m_conn_ptr->close();
            m_conn_ptr.reset();
            m_connected = false;
        }
    }

    hc::api::json::login_api_response api_request_maker::login(const std::string& username, const std::string& password) {
        hc::api::json::login_api_request req;
        req.set_username(username);
        req.set_password(password);

        hc::api::json::login_api_response res;
        make_request("POST", "/login", req, res);

        m_logged_in = true;
        m_token = res.get_token();

        return res;
    }

    hc::api::json::register_device_api_response api_request_maker::register_device(const std::string& device_name, const std::string& device_type) {
        hc::api::json::register_device_api_request req;
        req.set_device_name(device_name);
        req.set_device_type(device_type);

        hc::api::json::register_device_api_response res;
        make_request("POST", "/register_device", req, res);
        
        return res;
    }

    void api_request_maker::make_request(const std::string& method, const std::string& url, const hc::api::json::api_message& request, hc::api::json::api_message& response_ref) {
        if (!m_connected) {
            throw hc::exception("not intialized", "hc::net::api::api_request_maker::make_request");
        }

        http::http_request http_req(method, url, request.str());
        add_headers(http_req);

        m_conn_ptr->send(http_req.str());
        std::string response = m_conn_ptr->recv();

        m_http_parser.parse(response);

        if (!response_ref.parse(m_http_parser.get_response().get_body())) {
            std::string err_str = response_ref.get_error_msg() + " (" + std::to_string(response_ref.get_error_code()) + ")";
            throw hc::exception(err_str, "hc::net::api::api_request_maker::make_request");
        }
    }

    void api_request_maker::add_headers(hc::http::http_request& request_ref) {
        request_ref.add_header("Host", endpoints::API_HOST);
        request_ref.add_header("Content-Length", std::to_string(request_ref.get_body().length()));
        request_ref.add_header("Content-Type", "application/json");
        request_ref.add_header("Connection", "keep-alive");

        if (m_logged_in) {
            request_ref.add_header("Authorization", info::NAME + " " + m_token);
        }
    }

}
}
}