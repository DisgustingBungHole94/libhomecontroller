#include "homecontroller/thread/thread_pool.h"

#include "homecontroller/util/logger.h"
#include "homecontroller/exception/exception.h"

#include <sstream>

namespace hc {
namespace thread {
    
    void thread_pool::start(int num_threads) {
        if (num_threads <= 0) {
            throw exception("invalid pool size", "hc::thread::thread_pool::start");
        }

        m_threads.resize(num_threads);

        for (int i = 0; i < num_threads; i++) {
            m_threads.at(i) = std::thread(&thread_pool::worker_loop, this);
        }

        util::logger::dbg("created " + std::to_string(num_threads) + " threads");
    }

    void thread_pool::stop() {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_should_terminate = true;

        lock.unlock();
        m_cv.notify_all();

        for (auto& t : m_threads) {
            t.join();
        }

        m_threads.clear();
    }

    void thread_pool::add_job(job job) {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_job_queue.push(job);
        m_cv.notify_one();
    }

    void thread_pool::worker_loop() {
        while(true) {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_cv.wait(lock, [this] {
                return !m_job_queue.empty() || m_should_terminate;
            });

            if (m_should_terminate) {
                return;
            }

            std::thread::id id = std::this_thread::get_id();

            util::logger::dbg("thread [" + thread_id_to_string(id) + "] started job");

            job job = m_job_queue.front();
            m_job_queue.pop();

            lock.unlock();
            job();

            util::logger::dbg("thread [" + thread_id_to_string(id) + "] finished job");
        }
    }

    std::string thread_pool::thread_id_to_string(std::thread::id id) {
        std::ostringstream ss;
        ss << id;

        return ss.str();
    }

}
}