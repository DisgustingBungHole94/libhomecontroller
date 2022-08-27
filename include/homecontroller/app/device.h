#pragma once

#include <string>
#include <memory>

namespace hc {
    class device {
        public:
            device() 
                : device("", "", "")
            {}

            device(const std::string& id, const std::string& name, const std::string& type)
                : m_id(id), m_name(name), m_type(type)
            {}

            ~device() {}

            void setId(const std::string& id) { m_id = id; }
            const std::string& getId() { return m_id; }

            void setName(const std::string& name) { m_name = name; }
            const std::string& getName() { return m_name; }

            void setType(const std::string& type) { m_type = type; }
            const std::string& getType() { return m_type; } 

        private:
            std::string m_id;
            
            std::string m_name;
            std::string m_type;
    };
}