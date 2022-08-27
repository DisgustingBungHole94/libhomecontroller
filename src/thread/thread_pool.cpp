#include "homecontroller/thread/thread_pool.h"

#include "homecontroller/exception/exception.h"

#include <sstream>

namespace hc {
namespace thread {
    
    void thread_pool::start(int numThreads) {
        if (numThreads <= 0) {
            throw hc::exception("invalid pool size", "hc::thread::thread_pool::start");
        }

        m_threads.resize(numThreads);

        for (int i = 0; i < numThreads; i++) {
            m_threads.at(i) = std::thread(&thread_pool::workerLoop, this);
        }

        m_logger.dbg("created " + std::to_string(numThreads) + " threads");
    }

    void thread_pool::stop() {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_shouldTerminate = true;

        lock.unlock();
        m_cv.notify_all();

        for (auto& t : m_threads) {
            t.join();
        }

        m_threads.clear();
    }

    void thread_pool::addJob(job job) {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_jobQueue.push(job);
        m_cv.notify_one();
    }

    void thread_pool::workerLoop() {
        while(true) {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_cv.wait(lock, [this] {
                return !m_jobQueue.empty() || m_shouldTerminate;
            });

            if (m_shouldTerminate) {
                return;
            }

            std::thread::id id = std::this_thread::get_id();

            m_logger.dbg("thread [" + threadIdToString(id) + "] started job");

            job job = m_jobQueue.front();
            m_jobQueue.pop();

            lock.unlock();
            job();

            m_logger.dbg("thread [" + threadIdToString(id) + "] finished job");
        }
    }

    std::string thread_pool::threadIdToString(std::thread::id id) {
        std::ostringstream ss;
        ss << id;

        return ss.str();
    }

}
}