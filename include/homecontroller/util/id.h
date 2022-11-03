#pragma once

#include <string>

namespace hc {
namespace util {
    
    class id {
        public:
            id() {}
            ~id() {}

            void generate();

            const std::string& str() { return m_id; }

        private:
            std::string m_id;
    };

}
}