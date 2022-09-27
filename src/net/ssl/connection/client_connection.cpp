#include "homecontroller/net/ssl/connection/client_connection.h"

#include "homecontroller/exception/exception.h"

namespace hc {
namespace net {
namespace ssl {

    bool client_connection::handshake() {
        int res = SSL_connect(m_ssl.get());

        if (res != 1) {
            return false;
        }

        m_ready = true;
        return true;
    }

    void client_connection::close() {
        _close();
    }

    client_conn_ptr client_connection::conn_from_hdl(client_conn_hdl hdl) {
        if (hdl.expired()) {
            throw exception("bad connection", "hc::net::ssl::client_connection::conn_from_hdl");
        }

        return hdl.lock();
    }

}
}
}