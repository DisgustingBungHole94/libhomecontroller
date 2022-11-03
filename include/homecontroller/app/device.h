#pragma once

#include "homecontroller/ws/connection/server_connection.h"
#include "homecontroller/app/device_type.h"

#include <string>
#include <memory>

namespace hc {
    class device {
        public:
            device() 
                : device("", "", device_type::UNKNOWN)
            {}

            device(const std::string& name, const device_type type)
                 : device("", name, type)
            {}

            device(const std::string& id, const std::string& name, device_type type)
                : m_id(id), m_name(name), m_type(type)
            {}

            ~device() {}

            void set_id(const std::string& id) { m_id = id; }
            const std::string& get_id() { return m_id; }

            void set_name(const std::string& name) { m_name = name; }
            const std::string& get_name() { return m_name; }

            void set_type(device_type type) { m_type = type; }
            device_type get_type() { return m_type; } 

            void set_connection(std::weak_ptr<hc::ws::server_connection> connection);
            std::shared_ptr<hc::ws::server_connection> get_connection();

        private:
            std::string m_id;
            
            std::string m_name;
            device_type m_type;

            std::weak_ptr<hc::ws::server_connection> m_connection;
    };
}