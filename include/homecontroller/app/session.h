#pragma once

#include "homecontroller/app/user.h"

#include <chrono>

namespace hc {
    class session {
        public:
            session()
                : session("", "", nullptr)
            {}

            session(const std::string& id, const std::string& ip, std::shared_ptr<user> user)
                : m_id(id), m_ip(ip), m_user(user)
            {
                update_last_use();
            }

            ~session() {}

            void set_id(const std::string& id) { m_id = id; }
            const std::string& get_id() { return m_id; }

            void set_ip(const std::string& ip) { m_ip = ip; }
            const std::string& get_ip() { return m_ip; }

            void set_user(std::weak_ptr<user>& user) { m_user = user; }
            std::shared_ptr<user> get_user();

            void update_last_use();
            std::chrono::time_point<std::chrono::high_resolution_clock> get_last_use() { return m_last_use; }

        private:
            std::string m_id;
            
            std::string m_ip;
            std::chrono::time_point<std::chrono::high_resolution_clock> m_last_use;

            std::weak_ptr<user> m_user;
    };
}