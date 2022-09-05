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

            user(const std::string& username, const std::string& password, int permission_level)
                : m_username(username), m_password(password), m_permission_level(permission_level)
            {}

            ~user() {}

            void set_username(const std::string& username) { m_username = username; }
            const std::string& get_username() { return m_username; }

            void set_password(const std::string& password) { m_password = password; }
            const std::string& get_password() { return m_password; }

            void set_permission_level(int permission_level) { m_permission_level = permission_level; }
            int get_permission_level() { return m_permission_level; }

            void add_device(const device& device);
            void remove_device(const std::string& id);

            std::shared_ptr<device> get_device(const std::string& id);

        private:
            std::string m_username;
            std::string m_password;

            int m_permission_level;

            std::map<std::string, std::shared_ptr<device>> m_devices;
    };
}