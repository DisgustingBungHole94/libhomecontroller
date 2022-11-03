#include "homecontroller/app/session.h"

#include "homecontroller/exception/exception.h"

namespace hc {
    std::shared_ptr<user> session::get_user() {
        if (m_user.expired()) {
            throw exception("user deleted", "hc::session::get_user");
        }

        return m_user.lock();
    }

    void session::update_last_use() {
        m_last_use = std::chrono::high_resolution_clock::now();
    }
}