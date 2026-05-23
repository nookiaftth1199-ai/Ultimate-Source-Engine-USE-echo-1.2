// ============================================================
// Ultimate Source Engine - Thread Pool Implementation
// ============================================================

#include "stdafx.h"
#include "ThreadPool.h"
#include "Logger.h"

namespace USE {

    // -----------------------------------------------------------------
    // Constructor
    // -----------------------------------------------------------------
    ThreadPool::ThreadPool(size_t numThreads)
        : m_stop(false)
        , m_busyThreads(0)
    {
        if (numThreads == 0) {
            numThreads = std::thread::hardware_concurrency();
            if (numThreads == 0) numThreads = 2; // fallback
        }

        m_workers.reserve(numThreads);
        for (size_t i = 0; i < numThreads; ++i) {
            m_workers.emplace_back(&ThreadPool::Worker, this);
        }

        USE_LOG_INFO("ThreadPool created with %zu threads.", numThreads);
    }

    // -----------------------------------------------------------------
    // Destructor
    // -----------------------------------------------------------------
    ThreadPool::~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_stop = true;
        }
        m_condition.notify_all();

        for (std::thread& worker : m_workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }

        USE_LOG_INFO("ThreadPool destroyed.");
    }

    // -----------------------------------------------------------------
    // Worker thread main loop
    // -----------------------------------------------------------------
    void ThreadPool::Worker()
    {
        while (true) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                m_condition.wait(lock, [this]() { return m_stop || !m_tasks.empty(); });

                if (m_stop && m_tasks.empty()) {
                    return;
                }

                task = std::move(m_tasks.front());
                m_tasks.pop();
            }

            // Execute task
            m_busyThreads++;
            try {
                task();
            } catch (const std::exception& e) {
                USE_LOG_ERROR("Exception in thread pool task: %s", e.what());
            } catch (...) {
                USE_LOG_ERROR("Unknown exception in thread pool task.");
            }
            m_busyThreads--;
        }
    }

    // -----------------------------------------------------------------
    // Get pending task count
    // -----------------------------------------------------------------
    size_t ThreadPool::GetPendingTaskCount() const
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        return m_tasks.size();
    }

    // -----------------------------------------------------------------
    // Wait for all tasks to complete
    // -----------------------------------------------------------------
    void ThreadPool::WaitAll()
    {
        // Wait until queue is empty and no threads are busy
        while (true) {
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                if (m_tasks.empty() && m_busyThreads == 0) {
                    break;
                }
            }
            std::this_thread::yield();
        }
    }

    // -----------------------------------------------------------------
    // Resize the pool (stop current threads and restart)
    // -----------------------------------------------------------------
    void ThreadPool::Resize(size_t newThreadCount)
    {
        // Stop current threads
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_stop = true;
        }
        m_condition.notify_all();

        for (std::thread& worker : m_workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }

        // Clear old workers
        m_workers.clear();
        m_stop = false;

        // Start new threads
        if (newThreadCount == 0) {
            newThreadCount = std::thread::hardware_concurrency();
            if (newThreadCount == 0) newThreadCount = 2;
        }

        m_workers.reserve(newThreadCount);
        for (size_t i = 0; i < newThreadCount; ++i) {
            m_workers.emplace_back(&ThreadPool::Worker, this);
        }

        USE_LOG_INFO("ThreadPool resized to %zu threads.", newThreadCount);
    }

} // namespace USE