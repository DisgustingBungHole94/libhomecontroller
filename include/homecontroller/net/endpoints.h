#pragma once

#include <string>

namespace hc {
namespace net {
namespace endpoints {
    
    static const std::string API_HOST       = "api.bunghole.lol";
    static const std::string API_PORT       = "4434";
    
    static const std::string HTTPS_ENDPOINT = "https://" + API_HOST + ":" + API_PORT;
    static const std::string WS_ENDPOINT    = "ws://" + API_HOST + ":" + API_PORT;

}
}
}