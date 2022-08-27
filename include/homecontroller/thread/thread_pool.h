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
                : m_logger("thread_pool"), m_shouldTerminate(false)    
            {}

            ~thread_pool() {}

            void start(int numThreads);
            void stop();

            void addJob(job job);

        private:
            void workerLoop();

            std::string threadIdToString(std::thread::id id);

            hc::util::logger m_logger;

            std::mutex m_mutex;
            std::condition_variable m_cv;

            std::vector<std::thread> m_threads;

            std::queue<job> m_jobQueue;

            bool m_shouldTerminate;
    };

}
}