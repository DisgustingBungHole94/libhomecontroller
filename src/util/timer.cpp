#include "homecontroller/util/timer.h"

namespace hc {
namespace util {

    template<typename duration_type>
    timer<duration_type>::timer(int duration) 
        : m_duration(duration) 
    {
        reset();
    }

    template<typename duration_type>
    void timer<duration_type>::reset() {
        m_start_time = std::chrono::high_resolution_clock::now();
    }

    template<typename duration_type>
    bool timer<duration_type>::finished() {
        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();

        if (std::chrono::duration_cast<duration_type>(now - m_start_time) > m_duration) {
            return true;
        }

        return false;
    }

    template class timer<std::chrono::microseconds>;
    template class timer<std::chrono::milliseconds>;
    template class timer<std::chrono::seconds>;
    template class timer<std::chrono::minutes>;
    template class timer<std::chrono::hours>;

}
}