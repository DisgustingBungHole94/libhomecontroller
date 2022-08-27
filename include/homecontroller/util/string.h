#pragma once

#include <string>
#include <vector>

namespace hc {
namespace util {
namespace str {

    extern std::string toLowerCase(std::string str);
    extern std::vector<std::string> splitString(std::string str, char delim);

}
}
}