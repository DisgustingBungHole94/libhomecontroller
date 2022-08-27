#include "homecontroller/http/http_parser.h"

#include "homecontroller/util/string.h"
#include "homecontroller/exception/exception.h"

namespace hc {
namespace http {

    void http_parser::init(parser_type type) {
        m_type = type;
        
        llhttp_type llhttpType;

        switch(m_type) {
            case parser_type::REQUEST:
                llhttpType = HTTP_REQUEST;
                break;
            case parser_type::RESPONSE:
                llhttpType = HTTP_RESPONSE;
                break;
        }

        llhttp_init(&m_parser, llhttpType, &m_settings);
        m_parser.data = &m_data;

        llhttp_settings_init(&m_settings);

        m_settings.on_url = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
            parser_data* dataPtr = (parser_data*)parser->data;
            dataPtr->m_url = std::string(at, len);

            return 0;
        };

        m_settings.on_status = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
            parser_data* dataPtr = (parser_data*)parser->data;
            dataPtr->m_status = std::string(at, len);

            return 0;
        };

        m_settings.on_body = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
            parser_data* dataPtr = (parser_data*)parser->data;
            dataPtr->m_body = std::string(at, len);

            return 0;
        };

        m_settings.on_header_field = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
            parser_data* dataPtr = (parser_data*)parser->data;
            
            std::string field = std::string(at, len);
            dataPtr->m_lastHeaderField = hc::util::str::toLowerCase(field);

            return 0;
        };

        m_settings.on_header_value = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
            parser_data* dataPtr = (parser_data*)parser->data;
            
            if (dataPtr->m_lastHeaderField != "") {
                dataPtr->m_headers.insert(std::make_pair(dataPtr->m_lastHeaderField, std::string(at, len)));
                dataPtr->m_lastHeaderField = "";
            }

            return 0;
        };

        m_settings.on_message_complete = [](llhttp_t* parser) -> int {
            parser_data* dataPtr = (parser_data*)parser->data;
            dataPtr->m_finished = true;

            return 0;
        };
    }

    bool http_parser::parse(const std::string& data) {
        llhttp_errno err = llhttp_execute(&m_parser, data.c_str(), data.length());
        if (err == HPE_PAUSED_UPGRADE) {
            m_data.m_upgrade = true;
        } else if (err != HPE_OK) {
            throw hc::exception("failed to parse HTTP: " + std::string(llhttp_errno_name(err)) + " " + std::string(m_parser.reason), "hc::http::http_parser::parse");
        }

        if (m_data.m_finished) {
            if (m_type == parser_type::REQUEST) { 
                m_data.m_method = std::string(llhttp_method_name((llhttp_method_t)m_parser.method));
            }
            return true;
        }

        return false;
    }

    hc::http::http_request http_parser::getRequest() {
        if (m_type != parser_type::REQUEST) {
            throw hc::exception("parser type is not request", "hc::http::http_parser::getRequest");
        }

        hc::http::http_request request(m_data.m_method, m_data.m_url, m_data.m_body);
        request.setShouldUpgrade(m_data.m_upgrade);

        m_data = parser_data();

        return request;
    }

    hc::http::http_response http_parser::getResponse() {
        if (m_type != parser_type::RESPONSE) {
            throw hc::exception("parser type is not response", "hc::http::http_parser::getResponse");
        }

        hc::http::http_response response(m_data.m_status, m_data.m_body, m_data.m_headers);
        response.setShouldUpgrade(m_data.m_upgrade);

        m_data = parser_data();

        return response;
    }

}
}