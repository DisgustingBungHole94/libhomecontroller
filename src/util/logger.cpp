#include "homecontroller/util/logger.h"

#include <iostream>

#include <ctime>
#include <sstream>
#include <iomanip>

namespace hc {
namespace util {

    const std::string logger::_MASTER_PREFIX = "[hc]";

    const std::string logger::_LOG_PREFIX = "[LOG]";
    const std::string logger::_ERR_PREFIX = "[ERROR]";
    const std::string logger::_CSH_PREFIX = "[FATAL]";
    const std::string logger::_DBG_PREFIX = "[DEBUG]";

    bool logger::_DEBUG_ENABLED = false;

    logger::logger(const std::string& name) 
        : m_name(name)
    {
        if (m_name.length() + 2 >= 16) {
            m_tab_str = "\t";
        } else if (m_name.length() + 2 >= 8) {
            m_tab_str = "\t\t";
        } else {
            m_tab_str = "\t\t\t";
        }
    }

    logger::~logger() {}

    void logger::log(const std::string msg, bool nl) {
        print(_LOG_PREFIX, msg, nl);
    }

    void logger::err(const std::string msg, bool nl) {
        print(_ERR_PREFIX, msg, nl, "\033[1;33m");
    }

    void logger::csh(const std::string msg, bool nl) {
        print(_CSH_PREFIX, msg, nl, "\033[1;31m");
    }

    void logger::dbg(const std::string msg, bool nl) {
        if (_DEBUG_ENABLED) {
            print(_DBG_PREFIX, msg, nl, "\033[0;36m");
        }
    }

    std::string logger::timestamp() {
        auto t = std::time(nullptr);

        std::ostringstream ss;
        ss << std::put_time(std::localtime(&t), "%m-%d-%y %H:%M:%S");

        return ss.str();
    }

    void logger::print(const std::string prefix, const std::string msg, bool nl, std::string color) {
        std::ostringstream ss;
        ss << _MASTER_PREFIX << "\t" << timestamp() << "\t[" << m_name << "]" << m_tab_str << prefix << "\t\t" << msg;
        if (nl) ss << "\n";

        std::cout << color + ss.str() + "\033[0m";
    }

}
}