#pragma once

#include <rapidjson/document.h>

#include <string>

namespace hc {
namespace util {
    
    class json_document {
        public:
            json_document() {}
            ~json_document() {}

            void parse(const std::string& json_str);

            std::string         get_string(const std::string& name);
            int                 get_int(const std::string& name);
            double              get_double(const std::string& name);
            bool                get_bool(const std::string& name);
            void                get_object(const std::string& name, json_document& doc_ref);

        private:
            rapidjson::Document m_rapidjson_document;
    };

}
}