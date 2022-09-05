#pragma once

#include <chrono>

namespace hc {
namespace util {

    template<typename duration_type>
    class timer {
        public:
            timer(int duration);
            
            ~timer() {}

            void reset();
            bool finished();

        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;

            duration_type m_duration;
    };

}
}