#include "homecontroller/http_parser.h"

#include "homecontroller/exception.h"

#include <rapidjson/error/en.h>

namespace hc {
    rapidjson::Document HTTPResponse::parseJSON() {
        rapidjson::Document document;
        if (m_body != "") {
            rapidjson::ParseResult res = document.Parse(m_body.c_str());

            if (!res) {
                throw Exception("failed to parse JSON: " + std::string(rapidjson::GetParseError_En(res.Code())) + " (" + std::to_string(res.Offset()) + ")", "Device::login");
            }
        } else {
            document.Parse("{}");
        }

        return document;
    }

    HTTPParser::HTTPParser() {}
    HTTPParser::~HTTPParser() {}

    void HTTPParser::init() {
        llhttp_init(&m_parser, HTTP_RESPONSE, &m_settings);

        llhttp_settings_init(&m_settings);

        m_settings.on_status = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
            HTTPParser::ParserData* dataPtr = (HTTPParser::ParserData*)parser->data;
            dataPtr->m_status = std::string(at, len);

            return 0;
        };

        m_settings.on_body = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
            HTTPParser::ParserData* dataPtr = (HTTPParser::ParserData*)parser->data;
            dataPtr->m_body = std::string(at, len);

            return 0;
        };
    }

    HTTPResponse HTTPParser::parse(const std::string& response) {
        std::unique_ptr<HTTPParser::ParserData> data = std::make_unique<HTTPParser::ParserData>();
        m_parser.data = data.get();

        data->m_upgrade = false;

        enum llhttp_errno err = llhttp_execute(&m_parser, response.c_str(), response.length());
        if (err == HPE_PAUSED_UPGRADE) {
            data->m_upgrade = true;
        } else if (err != HPE_OK) {
            m_parser.data = nullptr;
            throw Exception("failed to parse HTTP request: " + std::string(llhttp_errno_name(err)) + " " + std::string(m_parser.reason), "HTTPParser::parse");
        }

        HTTPResponse httpResponse(data->m_status, data->m_body, data->m_upgrade);
        m_parser.data = nullptr;

        return httpResponse;
    }
}