#include "homecontroller/app/user.h"

#include "homecontroller/exception/exception.h"

namespace hc {
    void user::add_device(const device& device) {
        std::shared_ptr<hc::device> ptr = std::make_shared<hc::device>(device);
        m_devices.insert(std::make_pair(ptr->get_id(), ptr));
    }

    void user::remove_device(const std::string& id) {
        auto mit = m_devices.find(id);
        if (mit == m_devices.end()) {
            throw exception("device does not exist", "hc::user::removeDevice");
        }

        m_devices.erase(mit);
    }

    std::shared_ptr<device> user::get_device(const std::string& id) {
        auto mit = m_devices.find(id);
        if (mit == m_devices.end()) {
            return nullptr;
        }

        return mit->second;
    }
}