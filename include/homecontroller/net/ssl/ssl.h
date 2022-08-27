#pragma once

#include <memory>
#include <string>

#include <openssl/ssl.h>
#include <openssl/bio.h>

namespace hc {
namespace net {
namespace ssl {

    template<class T> struct deleter_of;
    template<> struct deleter_of<SSL_CTX>   { void operator()(SSL_CTX* p) const     { SSL_CTX_free(p); } };
    template<> struct deleter_of<SSL>       { void operator()(SSL* p) const         { SSL_free(p); } };
    template<> struct deleter_of<BIO>       { void operator()(BIO* p) const         { BIO_free_all(p); } };
    template<> struct deleter_of<BIO_METHOD>{ void operator()(BIO_METHOD* p) const  { BIO_meth_free(p); } };

    template<class openssl_type>
    using unique_ptr = std::unique_ptr<openssl_type, deleter_of<openssl_type>>;

    class string_bio {
        public:
            string_bio(string_bio&) = delete;
            string_bio& operator=(string_bio&) = delete;

            explicit string_bio();

            BIO* getBIO() { return m_bio.get(); }
            const std::string& getStr() { return m_str; }

        private:
            std::string m_str;

            hc::net::ssl::unique_ptr<BIO_METHOD> m_methods;
            hc::net::ssl::unique_ptr<BIO> m_bio;
    };

    extern std::string errorStr();

}
}
}