#include "homecontroller/net/ssl/tls_connection.h"

#include "homecontroller/exception/socket_timeout_exception.h"
#include "homecontroller/exception/socket_close_exception.h"

#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <vector>
#include <iostream>

namespace hc {
namespace net {
namespace ssl {
    
    void tls_connection::handshake() {
        int res;

        switch(m_mode) {
            case tls_connection_mode::SERVER:
                res = SSL_accept(m_ssl.get());
                break;
            case tls_connection_mode::CLIENT:
                res = SSL_connect(m_ssl.get());
                break;
            default:
                throw exception("unknown connection type", "hc::net::ssl::tls_connection_handshake");
        }

        if (res != 1) {
            int sslErr = SSL_get_error(m_ssl.get(), res);

            switch(sslErr) {
                case SSL_ERROR_WANT_READ:
                case SSL_ERROR_WANT_WRITE:
                    break;
                default:
                    close();
                    break;
            }
        } else {
            m_ready = true;
        }
    }

    std::string tls_connection::recv() {
        if (m_closed) {
            throw exception("socket is closed", "hc::net::ssl::tls_connection::recv");
        }

        static const unsigned int MAX_BUF_LEN = 4096;
        std::vector<unsigned char> buf(MAX_BUF_LEN);

        std::string data;

        int numBytes = 0;
        do {
            numBytes = SSL_read(m_ssl.get(), &buf[0], buf.size());

            if (numBytes < 0) {
                m_closed = true;
                throw exception("failed to receive bytes", "hc::net::ssl::tls_client::recv");
            } else if (numBytes == 0) {
                m_closed = true;
            }

            buf.resize(numBytes);
            data.append(buf.cbegin(), buf.cend());
        } while(numBytes == MAX_BUF_LEN);

        return data;
    }

    void tls_connection::send(const std::string& data) {
        if (m_closed) {
            throw exception("socket is closed", "hc::net::ssl::tls_connection::send");
        }

        if (SSL_write(m_ssl.get(), data.c_str(), data.length()) < 0) {
            throw exception("failed to send bytes", "hc::net::ssl::tls_client::send");
        }
    }

    void tls_connection::close() {
        if (m_mode == tls_connection_mode::SERVER) {
            uint64_t d = 1;
            
            int numBytes = write(m_close_event_fd, &d, sizeof(uint64_t));
            if (numBytes != sizeof(uint64_t)) {
                throw exception("failed to write to close event fd", "hc::net::ssl::tls_connection::close");
            }

            m_needs_close = true;
        } else {
            _close();
        }
    }

    void tls_connection::_close() {
        if (!m_closed) {
            m_closed = true;

            SSL_shutdown(m_ssl.get());

            shutdown(get_socket(), SHUT_RDWR);
            ::close(get_socket());
        }
    }

    int tls_connection::time_since_create() {
        auto now = std::chrono::high_resolution_clock::now();

        return std::chrono::duration_cast<std::chrono::microseconds>(now - m_create_time).count();
    }

    connection_ptr tls_connection::conn_from_hdl(connection_hdl hdl) {
        if (hdl.expired()) {
            throw exception("bad connection", "hc::net::ssl::tls_server::conn_from_hdl");
        }
        
        return hdl.lock();
    }

}
}
}