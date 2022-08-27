#pragma once

#include <websocketpp/config/core.hpp>
#include <websocketpp/server.hpp>

#include <websocketpp/config/core_client.hpp>
#include <websocketpp/client.hpp>

namespace hc {
namespace ws {

    struct server_config : public websocketpp::config::core {
        typedef websocketpp::transport::iostream::endpoint<websocketpp::config::core::transport_config> transport_type;
    };

    typedef websocketpp::server<server_config> server;
    
    struct client_config : public websocketpp::config::core {
        typedef websocketpp::transport::iostream::endpoint<websocketpp::config::core_client::transport_config> transport_type;
    };

    typedef websocketpp::client<client_config> client;

}
}