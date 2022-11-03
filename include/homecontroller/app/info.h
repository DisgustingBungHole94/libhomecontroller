#pragma once

#include <string>

namespace hc {
namespace info {

    static const std::string NAME        = "HomeController";
    static const std::string VERSION     = "3.0.0";

#ifdef _WIN32
    static const std::string OS          = "Windows";
#endif
#if defined(unix) || defined(__unix__) || defined(__unix)
    static const std::string OS          = "Linux";
#endif
#ifdef __APPLE__
    static const std::string OS          = "macOS";
#endif

}
}