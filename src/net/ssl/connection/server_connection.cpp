#include "homecontroller/net/ssl/connection/server_connection.h"

#include "homecontroller/exception/exception.h"

#include <unistd.h>

namespace hc {
namespace net {
namespace ssl {

    bool server_connection::handshake() {
        int res = SSL_accept(m_ssl.get());

        if (res != 1) {
            int ssl_err = SSL_get_error(m_ssl.get(), res);

            switch(ssl_err) {
                case SSL_ERROR_WANT_READ:
                case SSL_ERROR_WANT_WRITE:
                    return true;
                    break;
                default:
                    return false;
                    break;
            }
        }

        m_ready = true;
        return true;
    }

    void server_connection::close() {
        uint64_t d = 1;
                
        int numBytes = write(m_close_event_fd, &d, sizeof(uint64_t));
        if (numBytes != sizeof(uint64_t)) {
            throw exception("failed to write to close event fd", "hc::net::ssl::tls_connection::close");
        }

        m_needs_close = true;
    }

    int server_connection::time_since_create() {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now - m_create_time).count();
    }

    void server_connection::reset_timer() {
        m_create_time = std::chrono::high_resolution_clock::now();
    }

    server_conn_ptr server_connection::conn_from_hdl(server_conn_hdl hdl) {
        if (hdl.expired()) {
            throw exception("bad connection", "hc::net::ssl::server_connection::conn_from_hdl");
        }

        return hdl.lock();
    }

}
}
}