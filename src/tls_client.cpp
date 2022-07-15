#include "homecontroller/tls_client.h"

#include "homecontroller/exception.h"

#include <sys/socket.h>
#include <cstring>
#include <netdb.h>
#include <unistd.h>

namespace hc {
    TLSClient::TLSClient() 
        : m_clientFinished(false)
    {}

    TLSClient::~TLSClient() {}
    
    void TLSClient::init() {
        const SSL_METHOD* method = TLS_client_method();

        m_sslContext.reset(SSL_CTX_new(method));
        if (m_sslContext == nullptr) {
            throw Exception("failed to init SSL context: " + getSSLError(), "TLSCLient::init");
        }

        m_ssl.reset(SSL_new(m_sslContext.get()));
        if (m_ssl == nullptr) {
            throw Exception("failed to init SSL: " + getSSLError(), "TLSCLient::init");
        }
    }

    void TLSClient::connect(const std::string& host, const std::string& port) {
        struct addrinfo hints = {}, *addrs;

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        int err = getaddrinfo(host.c_str(), port.c_str(), &hints, &addrs);
        if (err != 0) {
            throw Exception("failed to resolve host: " + std::string(gai_strerror(err)), "TLSClient::connect");
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
            throw Exception("failed to connect to server: " + std::string(strerror(err)), "TLSClient::connect");
        }

        m_clientSocket = sd;
        SSL_set_fd(m_ssl.get(), m_clientSocket);

        int res = SSL_connect(m_ssl.get());
        if (res != 1) {
            throw Exception("tls handshake failed", "TLSClient::connect");
        }
    }

    void TLSClient::send(const std::string& data) {
        if (SSL_write(m_ssl.get(), data.c_str(), data.length()) < 0) {
            throw Exception("failed to send bytes", "TLSClient::send");
        }
    }

    std::string TLSClient::recv() {
        const unsigned int MAX_BUF_LEN = 4096;
        std::vector<unsigned char> buf(MAX_BUF_LEN);

        std::string data;

        int numBytes = 0;
        do {
            numBytes = SSL_read(m_ssl.get(), &buf[0], buf.size());

            int res = SSL_get_error(m_ssl.get(), numBytes);
            if (res == SSL_ERROR_WANT_READ) {
                throw Exception("socket timed out", "TLSClient::recv");
            }

            if (numBytes < 0) {
                m_clientFinished = true;
                throw Exception("failed to receive bytes", "TLSClient::recv");
            } else if (numBytes == 0) {
                throw Exception("socket closed", "TLSClient::recv");
            }

            buf.resize(numBytes);
            data.append(buf.cbegin(), buf.cend());
        } while(numBytes == MAX_BUF_LEN);

        return data;
    }

    void TLSClient::stop() {
        if (!m_clientFinished) {
            m_clientFinished = true;

            SSL_shutdown(m_ssl.get());
            shutdown(m_clientSocket, SHUT_RDWR);
            close(m_clientSocket);
        }
    }

    std::string TLSClient::getSSLError() {
        char errBuf[256];
        ERR_error_string_n(ERR_get_error(), errBuf, 256);

        return std::string(errBuf);
    }
}