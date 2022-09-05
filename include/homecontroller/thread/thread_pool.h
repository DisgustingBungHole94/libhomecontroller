#pragma once

#include "homecontroller/util/logger.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>

namespace hc {
namespace thread {

    class thread_pool {
        public:
            typedef std::function<void()> job;

            thread_pool() 
                : m_logger("thread_pool"), m_should_terminate(false)    
            {}

            ~thread_pool() {}

            void start(int num_threads);
            void stop();

            void add_job(job job);

        private:
            void worker_loop();

            std::string thread_id_to_string(std::thread::id id);

            util::logger m_logger;

            std::mutex m_mutex;
            std::condition_variable m_cv;

            std::vector<std::thread> m_threads;

            std::queue<job> m_job_queue;

            bool m_should_terminate;
    };

}
}