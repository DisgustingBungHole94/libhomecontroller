#include "homecontroller/net/ssl/ssl.h"

#include "homecontroller/exception/exception.h"

#include <openssl/err.h>

namespace hc {
namespace net {
namespace ssl {

    string_bio::string_bio() {
        m_methods.reset(BIO_meth_new(BIO_TYPE_SOURCE_SINK, "string_bio"));
        if (m_methods == nullptr) {
            throw hc::exception("failed to create methods for string_bio", "hc::net::ssl::string_bio::string_bio");
        }

        BIO_meth_set_write(m_methods.get(), [](BIO* bio, const char* data, int len) -> int {
            std::string* str = reinterpret_cast<std::string*>(BIO_get_data(bio));
            str->append(data, len);

            return len;
        });

        m_bio.reset(BIO_new(m_methods.get()));
        if (m_bio == nullptr) {
            throw hc::exception("failed to create new bio from string_bio", "hc::net::ssl::string_bio::string_bio");
        }

        BIO_set_data(m_bio.get(), &m_str);
        BIO_set_init(m_bio.get(), 1);
    }

    std::string errorStr() {
        hc::net::ssl::string_bio bio;
        ERR_print_errors(bio.getBIO());

        return bio.getStr();
    }

}
}
}