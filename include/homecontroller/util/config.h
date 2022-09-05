#pragma once

#include <homecontroller/exception/exception.h>

#include <string>
#include <map>

namespace hc {
namespace util {

    class config_value {
        public:
            config_value(const std::string& name) 
                : m_name(name), m_str(""), m_int(0), m_is_string(false), m_is_int(false)
            {}

            ~config_value() {}

            void set_string(const std::string& val);
            const std::string& get_string();

            void set_int(int val);
            const int get_int();

            bool is_string() { return m_is_string; }
            bool is_int() { return m_is_int; }

        private:
            std::string m_name;

            std::string m_str;
            int m_int;

            bool m_is_string;
            bool m_is_int;
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