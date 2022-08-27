#pragma once

#include "homecontroller/app/user.h"

#include <chrono>

namespace hc {
    class session {
        public:
            session()
                : session("", "", nullptr)
            {}

            session(const std::string& id, const std::string& ip, std::shared_ptr<hc::user> user)
                : m_id(id), m_ip(ip), m_user(user)
            {
                updateLastUse();
            }

            ~session() {}

            void setId(const std::string& id) { m_id = id; }
            const std::string& getId() { return m_id; }

            void setIp(const std::string& ip) { m_ip = ip; }
            const std::string& getIp() { return m_ip; }

            void setUser(std::weak_ptr<hc::user>& user) { m_user = user; }
            std::shared_ptr<hc::user> getUser();

            void updateLastUse();
            std::chrono::time_point<std::chrono::high_resolution_clock> getLastUse() { return m_lastUse; }

        private:
            std::string m_id;
            
            std::string m_ip;
            std::chrono::time_point<std::chrono::high_resolution_clock> m_lastUse;

            std::weak_ptr<hc::user> m_user;
    };
}