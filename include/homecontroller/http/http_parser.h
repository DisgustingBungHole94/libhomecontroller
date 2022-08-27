#pragma once

#include "homecontroller/http/http_request.h"
#include "homecontroller/http/http_response.h"

#include <llhttp.h>

#include <string>
#include <map>

namespace hc {
namespace http {
    
    class http_parser {
        public:
            enum class parser_type {
                REQUEST, RESPONSE
            };

            http_parser() {}

            ~http_parser() {}

            void init(parser_type type);
            bool parse(const std::string& data);

            hc::http::http_request getRequest();
            hc::http::http_response getResponse();

            bool finished() { return m_data.m_finished; }

        private:
            llhttp_t m_parser;
            llhttp_settings_t m_settings;

            parser_type m_type;
            
            struct parser_data {
                std::string m_url = "";
                std::string m_method = "";

                std::string m_status = "";

                std::string m_body = "";

                std::string m_lastHeaderField = "";
                std::map<std::string, std::string> m_headers;

                bool m_upgrade = false;

                bool m_finished = false;
            } m_data;
    };

}
}