#include "homecontroller/app/session.h"

#include "homecontroller/exception/exception.h"

namespace hc {
    std::shared_ptr<hc::user> session::getUser() {
        if (m_user.expired()) {
            throw hc::exception("user deleted", "hc::session::getUser");
        }

        return m_user.lock();
    }

    void session::updateLastUse() {
        m_lastUse = std::chrono::high_resolution_clock::now();
    }
}