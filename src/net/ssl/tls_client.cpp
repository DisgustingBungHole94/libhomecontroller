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

        m_sslCtx.reset(SSL_CTX_new(method));
        if (m_sslCtx == nullptr) {
            throw hc::exception("failed to init SSL context", "hc::net::ssl::tls_context::init");
        }
    }

    std::unique_ptr<hc::net::ssl::tls_connection> tls_client::connect(const std::string& host, const std::string& port) {
        hc::net::ssl::unique_ptr<SSL> ssl;

        ssl.reset(SSL_new(m_sslCtx.get()));
        if (ssl == nullptr) {
            throw hc::exception("failed to create ssl", "hc::net::ssl::tls_context::connect");
        }
        
        struct addrinfo hints = {}, *addrs;

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        int err = getaddrinfo(host.c_str(), port.c_str(), &hints, &addrs);
        if (err != 0) {
            throw hc::exception("failed to resolve host", "hc::net::ssl::tls_context::connect");
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
            throw hc::exception("failed to connect to server", "hc::net::ssl::tls_context::connect");
        }

        SSL_set_fd(ssl.get(), sd);

        int res = SSL_connect(ssl.get());
        if (res != 1) {
            if (res == 0) {
                close(sd);
            }

            throw hc::exception("tls handshake failed", "hc::net::ssl::tls_context::connect");
        }

        std::unique_ptr<hc::net::ssl::tls_connection> conn = std::make_unique<hc::net::ssl::tls_connection>(std::move(ssl), hc::net::ssl::tls_connection_mode::CLIENT);

        return std::move(conn);
    }

}
}
}