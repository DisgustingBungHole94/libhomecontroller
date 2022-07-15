#pragma once

#include <string>
#include <rapidjson/document.h>
#include <llhttp.h>

namespace hc {
    class HTTPResponse {
        public:
            HTTPResponse()
                : HTTPResponse(0, "", false)
            {}

            HTTPResponse(const std::string& status, const std::string& body, bool shouldUpgrade)
                : m_status(status), m_body(body), m_shouldUpgrade(shouldUpgrade)
            {}

            ~HTTPResponse() {}

            rapidjson::Document parseJSON();

            const std::string& getStatus() { return m_status; }
            const std::string& getBody() { return m_body; }

            bool shouldUpgrade() { return m_shouldUpgrade; }

        private:
            std::string m_status;
            std::string m_body;

            bool m_shouldUpgrade;
    };

    class HTTPParser {
        public:
            HTTPParser();
            ~HTTPParser();

            void init();

            HTTPResponse parse(const std::string& response);

        private:
            llhttp_t m_parser;
            llhttp_settings_t m_settings;

            struct ParserData {
                std::string m_status;
                std::string m_body;

                bool m_upgrade;
            };
    };
}