#pragma once

#include "homecontroller/exception/exception.h"

namespace hc {
    class socket_timeout_exception : public hc::exception {
        public:
            socket_timeout_exception(const std::string& func)
                : exception("socket timed out", func)
            {}

            ~socket_timeout_exception() {}
    };
}