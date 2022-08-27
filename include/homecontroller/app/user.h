#pragma once

#include "device.h"

#include <string>
#include <map>
#include <memory>

namespace hc {
    class user {
        public:
            user()
                : user("", "", 0)
            {}

            user(const std::string& username, const std::string& password, int permissionLevel)
                : m_username(username), m_password(password), m_permissionLevel(permissionLevel)
            {}

            ~user() {}

            void setUsername(const std::string& username) { m_username = username; }
            const std::string& getUsername() { return m_username; }

            void setPassword(const std::string& password) { m_password = password; }
            const std::string& getPassword() { return m_password; }

            void setPermissionLevel(int permissionLevel) { m_permissionLevel = permissionLevel; }
            int getPermissionLevel() { return m_permissionLevel; }

            void addDevice(const hc::device& device);
            void removeDevice(const std::string& id);

            std::shared_ptr<hc::device> getDevice(const std::string& id);

        private:
            std::string m_username;
            std::string m_password;

            int m_permissionLevel;

            std::map<std::string, std::shared_ptr<hc::device>> m_devices;
    };
}