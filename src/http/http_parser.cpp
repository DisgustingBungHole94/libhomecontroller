#include "homecontroller/http/http_parser.h"

#include "homecontroller/util/string.h"
#include "homecontroller/exception/exception.h"

namespace hc {
namespace http {

    void http_parser::init(parser_type type) {
        m_type = type;
        
        llhttp_type llhttp_type;

        switch(m_type) {
            case parser_type::REQUEST:
                llhttp_type = HTTP_REQUEST;
                break;
            case parser_type::RESPONSE:
                llhttp_type = HTTP_RESPONSE;
                break;
        }

        llhttp_init(&m_parser, llhttp_type, &m_settings);
        m_parser.data = &m_data;

        llhttp_settings_init(&m_settings);

        m_settings.on_url = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
            parser_data* data_ptr = (parser_data*)parser->data;
            data_ptr->m_url = std::string(at, len);

            return 0;
        };

        m_settings.on_status = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
            parser_data* data_ptr = (parser_data*)parser->data;
            data_ptr->m_status = std::string(at, len);

            return 0;
        };

        m_settings.on_body = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
            parser_data* data_ptr = (parser_data*)parser->data;
            data_ptr->m_body = std::string(at, len);

            return 0;
        };

        m_settings.on_header_field = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
            parser_data* data_ptr = (parser_data*)parser->data;
            
            std::string field = std::string(at, len);
            data_ptr->m_last_header_field = util::str::to_lower_case(field);

            return 0;
        };

        m_settings.on_header_value = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
            parser_data* data_ptr = (parser_data*)parser->data;
            
            if (data_ptr->m_last_header_field != "") {
                data_ptr->m_headers.insert(std::make_pair(data_ptr->m_last_header_field, std::string(at, len)));
                data_ptr->m_last_header_field = "";
            }

            return 0;
        };

        m_settings.on_message_complete = [](llhttp_t* parser) -> int {
            parser_data* data_ptr = (parser_data*)parser->data;
            data_ptr->m_finished = true;

            return 0;
        };
    }

    bool http_parser::parse(const std::string& data) {
        llhttp_errno err = llhttp_execute(&m_parser, data.c_str(), data.length());
        if (err == HPE_PAUSED_UPGRADE) {
            m_data.m_upgrade = true;
        } else if (err != HPE_OK) {
            throw exception("failed to parse HTTP: " + std::string(llhttp_errno_name(err)) + " " + std::string(m_parser.reason), "hc::http::http_parser::parse");
        }

        if (m_data.m_finished) {
            if (m_type == parser_type::REQUEST) { 
                m_data.m_method = std::string(llhttp_method_name((llhttp_method_t)m_parser.method));
            }
            return true;
        }

        return false;
    }

  http_request http_parser::get_request() {
        if (m_type != parser_type::REQUEST) {
            throw exception("parser type is not request", "hc::http::http_parser::getRequest");
        }

        http_request request(m_data.m_method, m_data.m_url, m_data.m_body);
        request.set_headers(m_data.m_headers);
        request.set_should_upgrade(m_data.m_upgrade);

        m_data = parser_data();

        return request;
    }

    http_response http_parser::get_response() {
        if (m_type != parser_type::RESPONSE) {
            throw exception("parser type is not response", "hc::http::http_parser::getResponse");
        }

        http_response response(m_data.m_status, m_data.m_body, m_data.m_headers);
        response.set_headers(m_data.m_headers);
        response.set_should_upgrade(m_data.m_upgrade);

        m_data = parser_data();

        return response;
    }

}
}