// ============================================================
// Ultimate Source Engine - Thread Pool
// ============================================================
//
// Manages a pool of worker threads for parallel task execution.
// Tasks are queued and executed as threads become available.
// ============================================================

#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <atomic>
#include <type_traits>

namespace USE {

    class ThreadPool {
    public:
        // Constructor: launches numThreads worker threads (default = hardware concurrency)
        explicit ThreadPool(size_t numThreads = std::thread::hardware_concurrency());

        // Destructor: joins all threads
        ~ThreadPool();

        // Enqueue a task to be executed by the pool. Returns a std::future for the result.
        template<class F, class... Args>
        auto Enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

        // Get the number of threads in the pool
        size_t GetThreadCount() const { return m_workers.size(); }

        // Get the number of pending tasks
        size_t GetPendingTaskCount() const;

        // Wait for all tasks to complete (blocks until queue empty and all tasks done)
        void WaitAll();

        // Resize the pool (stops current threads and restarts with new count)
        void Resize(size_t newThreadCount);

    private:
        // Worker threads
        std::vector<std::thread> m_workers;

        // Task queue
        std::queue<std::function<void()>> m_tasks;

        // Synchronization
        mutable std::mutex m_queueMutex;
        std::condition_variable m_condition;
        std::atomic<bool> m_stop;
        std::atomic<size_t> m_busyThreads; // number of threads currently executing tasks

        // Main worker loop
        void Worker();
    };

    // -----------------------------------------------------------------
    // Template implementation (must be in header)
    // -----------------------------------------------------------------
    template<class F, class... Args>
    auto ThreadPool::Enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> result = task->get_future();

        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            if (m_stop) {
                throw std::runtime_error("Enqueue on stopped ThreadPool");
            }
            m_tasks.emplace([task]() { (*task)(); });
        }

        m_condition.notify_one();
        return result;
    }

} // namespace USE