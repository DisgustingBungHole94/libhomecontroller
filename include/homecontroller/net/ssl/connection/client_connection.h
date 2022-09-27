#pragma once

#include "connection.h"

namespace hc {
namespace net {
namespace ssl {

    class client_connection;

    typedef std::shared_ptr<client_connection> client_conn_ptr;
    typedef std::weak_ptr<client_connection> client_conn_hdl;

    class client_connection : public connection {
        public:
            client_connection(unique_ptr<SSL> ssl)
                : connection(std::move(ssl))
            {}

            ~client_connection() {}

            bool handshake();

            void close();

            void set_uri(const std::string& uri) { m_uri = uri; }
            const std::string& get_uri() { return m_uri; }

            static client_conn_ptr conn_from_hdl(client_conn_hdl hdl);
        
        private:
            std::string m_uri;
    };

}
}
}