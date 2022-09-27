#include "homecontroller/net/ssl/connection/connection.h"

#include "homecontroller/exception/exception.h"

#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <vector>

namespace hc {
namespace net {
namespace ssl {
    
    bool connection::handshake() {
        return false;
    }

    std::string connection::recv() {
        if (m_closed) {
            throw exception("socket is closed", "hc::net::ssl::tls_connection::recv");
        }

        int num_bytes = 0;
        std::string data;

        static const unsigned int MAX_BUF_LEN = 4096;
        std::vector<unsigned char> buf(MAX_BUF_LEN);

        do {
            num_bytes = SSL_read(m_ssl.get(), &buf[0], buf.size());

            if (num_bytes < 0) {
                m_closed = true;
                throw exception("failed to receive bytes", "hc::net::ssl::tls_client::recv");
            } else if (num_bytes == 0) {
                m_closed = true;
                return "";
            }

            buf.resize(num_bytes);
            data.append(buf.cbegin(), buf.cend());
        } while(num_bytes == MAX_BUF_LEN);

        return data;
    }

    void connection::send(const std::string& data) {
        if (m_closed) {
            throw exception("socket is closed", "hc::net::ssl::tls_connection::send");
        }

        if (SSL_write(m_ssl.get(), data.c_str(), data.size()) < 0) {
            throw exception("failed to send bytes", "hc::net::ssl::tls_client::send");
        }
    }

    void connection::close() {}

    void connection::_close() {
        if (!m_closed) {
            m_closed = true;

            SSL_shutdown(m_ssl.get());

            shutdown(get_socket(), SHUT_RDWR);
            ::close(get_socket());
        }
    }

    conn_ptr connection::conn_from_hdl(conn_hdl hdl) {
        if (hdl.expired()) {
            throw exception("bad connection", "hc::net::ssl::connection::conn_from_hdl");
        }

        return hdl.lock();
    }    

}
}
}