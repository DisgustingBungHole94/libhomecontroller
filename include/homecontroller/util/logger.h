#pragma once

#include <string>

namespace hc {
namespace util {
    class logger {
        public:
            logger(const std::string& name);
            ~logger();

            void log(const std::string msg, bool nl = true);
            void err(const std::string msg, bool nl = true);
            void csh(const std::string msg, bool nl = true);
            void dbg(const std::string msg, bool nl = true);

            static void enable_debug() { _DEBUG_ENABLED = true; }
            static void disable_debug() { _DEBUG_ENABLED = false;}

        private:
            static const std::string _MASTER_PREFIX;

            static const std::string _LOG_PREFIX;
            static const std::string _ERR_PREFIX;
            static const std::string _CSH_PREFIX;
            static const std::string _DBG_PREFIX;

            static bool _DEBUG_ENABLED;

            std::string m_name;
            std::string m_tab_str;

            std::string timestamp();

            void print(const std::string prefix, const std::string msg, bool nl, std::string color = "");
        };
}
}
