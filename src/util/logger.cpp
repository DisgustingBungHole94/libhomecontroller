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

    const std::string logger::_INPUT_PREFIX = "[INPUT]";

    bool logger::_DEBUG_ENABLED = false;

    void logger::log(const std::string msg, bool nl) {
        print(_LOG_PREFIX, msg, nl);
    }

    void logger::err(const std::string msg, bool nl) {
        print(_ERR_PREFIX, msg, nl, "\033[0;93m");
    }

    void logger::csh(const std::string msg, bool nl) {
        print(_CSH_PREFIX, msg, nl, "\033[0;91m");
    }

    void logger::dbg(const std::string msg, bool nl) {
        if (_DEBUG_ENABLED) {
            print(_DBG_PREFIX, msg, nl, "\033[3;90m");
        }
    }

    std::string logger::timestamp() {
        auto t = std::time(nullptr);

        std::ostringstream ss;
        ss << std::put_time(std::localtime(&t), "%m-%d-%y %H:%M:%S");

        return ss.str();
    }

    std::string logger::input(const std::string prompt) {
        print(_INPUT_PREFIX, prompt, false);

        std::string in;
        std::getline(std::cin, in);

        return in;
    }

    void logger::print(const std::string prefix, const std::string msg, bool nl, std::string color) {
        std::ostringstream ss;
        ss << _MASTER_PREFIX << " " << timestamp() << " " << prefix << "\t\t" << msg;
        if (nl) ss << "\n";

        std::cout << color + ss.str() + "\033[0m";
    }

}
}