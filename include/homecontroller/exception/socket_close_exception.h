#pragma once

#include "homecontroller/exception/exception.h"

namespace hc {
    class socket_close_exception : public exception {
        public:
            socket_close_exception(const std::string& func)
                : exception("socket closed", func)
            {}

            ~socket_close_exception() {}
    };
}