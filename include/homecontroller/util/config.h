#pragma once

#include <homecontroller/exception/exception.h>

#include <string>
#include <map>

namespace hc {
namespace util {

    class config_value {
        public:
            config_value(const std::string& name) 
                : m_name(name), m_str(""), m_int(0), m_isString(false), m_isInt(false)
            {}

            ~config_value() {}

            void setString(const std::string& val);
            const std::string& getString();

            void setInt(int val);
            const int getInt();

            bool isString() { return m_isString; }
            bool isInt() { return m_isInt; }

        private:
            std::string m_name;

            std::string m_str;
            int m_int;

            bool m_isString;
            bool m_isInt;
    };

    class config {
        public:
            config() {}

            ~config() {}

            void load(const std::string& path);

            config_value& operator[](const std::string& var);

        private:
            std::map<std::string, config_value> m_vars;
    };

}
}