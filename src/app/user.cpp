#include "homecontroller/app/user.h"

#include "homecontroller/exception/exception.h"

namespace hc {
    void user::addDevice(const hc::device& device) {
        std::shared_ptr<hc::device> ptr = std::make_shared<hc::device>(device);
        m_devices.insert(std::make_pair(ptr->getId(), ptr));
    }

    void user::removeDevice(const std::string& id) {
        auto mit = m_devices.find(id);
        if (mit == m_devices.end()) {
            throw hc::exception("device does not exist", "hc::user::removeDevice");
        }

        m_devices.erase(mit);
    }

    std::shared_ptr<hc::device> user::getDevice(const std::string& id) {
        auto mit = m_devices.find(id);
        if (mit == m_devices.end()) {
            return nullptr;
        }

        return mit->second;
    }
}