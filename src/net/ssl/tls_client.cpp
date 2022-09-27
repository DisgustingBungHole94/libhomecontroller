#include "homecontroller/net/ssl/tls_client.h"

#include "homecontroller/exception/exception.h"

#include <sys/socket.h>
#include <cstring>
#include <netdb.h>
#include <unistd.h>

namespace hc {
namespace net {
namespace ssl {
    
    void tls_client::init() {
        const SSL_METHOD* method = TLS_client_method();

        m_ssl_ctx.reset(SSL_CTX_new(method));
        if (m_ssl_ctx == nullptr) {
            throw exception("failed to init SSL context", "hc::net::ssl::tls_context::init");
        }
    }

    client_conn_ptr tls_client::connect(const std::string& host, const std::string& port) {
        unique_ptr<SSL> ssl;

        ssl.reset(SSL_new(m_ssl_ctx.get()));
        if (ssl == nullptr) {
            throw exception("failed to create ssl", "hc::net::ssl::tls_context::connect");
        }
        
        struct addrinfo hints = {}, *addrs;

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        int err = getaddrinfo(host.c_str(), port.c_str(), &hints, &addrs);
        if (err != 0) {
            throw exception("failed to resolve host", "hc::net::ssl::tls_context::connect");
        }

        int sd;
        for (struct addrinfo* addr = addrs; addr != nullptr; addr = addr->ai_next) {
            sd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
            if (sd == -1) {
                err = errno;
                break;
            }

            if (::connect(sd, addr->ai_addr, addr->ai_addrlen) == 0) {
                break;
            }

            err = errno;

            close(sd);
            sd = -1;
        }

        freeaddrinfo(addrs);

        if (sd == -1) {
            throw exception("failed to connect to server", "hc::net::ssl::tls_context::connect");
        }

        SSL_set_fd(ssl.get(), sd);

        int res = SSL_connect(ssl.get());
        if (res != 1) {
            if (res == 0) {
                close(sd);
            }

            throw exception("tls handshake failed", "hc::net::ssl::tls_context::connect");
        }

        return std::make_shared<client_connection>(std::move(ssl));
    }

}
}
}