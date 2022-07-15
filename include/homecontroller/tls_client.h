#pragma once

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <memory>
#include <vector>
#include <string>

namespace hc {
    namespace tls {
        template<class T> struct DeleterOf;
        template<> struct DeleterOf<SSL_CTX> { void operator()(SSL_CTX* p) const { SSL_CTX_free(p); } };
        template<> struct DeleterOf<SSL> { void operator()(SSL* p) const { SSL_free(p); } };

        template<class OpenSSLType>
        using UniquePtr = std::unique_ptr<OpenSSLType, tls::DeleterOf<OpenSSLType>>;
    }

    class TLSClient {
        public:
            TLSClient();
            ~TLSClient();

            void init();

            void connect(const std::string& host, const std::string& port);

            void send(const std::string& data);
            std::string recv();

            void stop();

        private:
            std::string getSSLError();

            int m_clientSocket;

            tls::UniquePtr<SSL_CTX> m_sslContext;
            tls::UniquePtr<SSL> m_ssl;

            bool m_clientFinished;
    };
}