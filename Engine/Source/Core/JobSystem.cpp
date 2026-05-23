// ============================================================
// Ultimate Source Engine - Job System Implementation
// ============================================================

#include "stdafx.h"
#include "JobSystem.h"
#include "Platform.h"
#include "Logger.h"

#include <algorithm>
#include <cassert>

namespace USE {

    // -----------------------------------------------------------------
    // ReadyQueue implementation
    // -----------------------------------------------------------------
    void JobSystem::ReadyQueue::Push(JobHandle job) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(job);
    }

    bool JobSystem::ReadyQueue::Pop(JobHandle& job) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty()) {
            return false;
        }
        job = m_queue.front();
        m_queue.pop();
        return true;
    }

    size_t JobSystem::ReadyQueue::Size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    // -----------------------------------------------------------------
    // JobSystem implementation
    // -----------------------------------------------------------------
    JobSystem::JobSystem()
        : m_stop(false)
        , m_numThreads(0)
        , m_nextJobId(1) // start from 1 (0 invalid)
        , m_pendingJobs(0)
    {
    }

    JobSystem::~JobSystem()
    {
        Shutdown();
    }

    bool JobSystem::Initialize(uint32_t numThreads)
    {
        if (numThreads == 0) {
            numThreads = Platform::GetCPUCount();
            if (numThreads < 2) numThreads = 2; // at least 2 workers
        }

        m_numThreads = numThreads;
        m_workers.reserve(numThreads);

        for (uint32_t i = 0; i < numThreads; ++i) {
            m_workers.emplace_back(&JobSystem::WorkerThread, this);
        }

        USE_LOG_INFO("JobSystem initialized with %u worker threads.", numThreads);
        return true;
    }

    void JobSystem::Shutdown()
    {
        if (m_workers.empty()) return;

        USE_LOG_INFO("Shutting down JobSystem...");

        m_stop = true;

        // Wake up all workers (they will check m_stop after popping queue)
        // We can push a special "wake" job or just let them time out? We'll use condition variable.
        // For simplicity, we'll just join threads; they will exit when m_stop is true and queue empty.
        // To speed up, we can push dummy jobs.
        for (size_t i = 0; i < m_workers.size(); ++i) {
            // Push a dummy job to wake a thread; each thread will see stop flag after finishing.
            // But if queue is empty, they wait on condition. We need to notify all.
        }
        // Actually, we'll just join; if threads are waiting, they need to be woken.
        // We'll add a condition variable to the ready queue? Not necessary: we can use a simple wake flag.

        // Better: have a per-worker condition? Let's keep simple: we'll rely on m_stop being checked before waiting.
        // But our ReadyQueue::Pop currently blocks with a condition variable? We didn't implement that.
        // Let's add a condition variable to the ready queue to allow waiting.

        // However, to keep this simpler, we'll have the worker thread loop with a small sleep if queue empty.
        // That's not efficient but okay for now. We'll revise later if needed.

        // For now, we'll just join and hope they exit.
        for (auto& t : m_workers) {
            if (t.joinable()) {
                t.join();
            }
        }

        m_workers.clear();

        // Clean up any remaining jobs
        {
            std::lock_guard<std::mutex> lock(m_jobMapMutex);
            m_jobs.clear();
        }

        USE_LOG_INFO("JobSystem shutdown complete.");
    }

    JobHandle JobSystem::CreateJob(const JobDesc& desc)
    {
        if (!desc.function) {
            USE_LOG_ERROR("JobSystem::CreateJob: null function");
            return 0;
        }

        JobHandle handle = GenerateJobId();

        auto job = std::make_unique<InternalJob>();
        job->id = handle;
        job->function = desc.function;
        job->userData = desc.userData;
        job->name = desc.name ? desc.name : "Unnamed";
        job->finished = false;
        job->started = false;
        job->unfinishedDependencies = desc.numDependencies;

        // Register job
        {
            std::lock_guard<std::mutex> lock(m_jobMapMutex);
            m_jobs[handle] = std::move(job);
        }

        // If there are dependencies, register this job as dependent on them
        if (desc.numDependencies > 0 && desc.dependencies) {
            std::lock_guard<std::mutex> lock(m_jobMapMutex); // protect job map while modifying dependents
            for (int i = 0; i < desc.numDependencies; ++i) {
                JobHandle dep = desc.dependencies[i];
                auto it = m_jobs.find(dep);
                if (it != m_jobs.end()) {
                    it->second->dependents.push_back(handle);
                } else {
                    USE_LOG_WARN("Job dependency not found: %llu", dep);
                    // If dependency doesn't exist, we should decrement? Actually, treat as already finished.
                    // We'll decrement counter.
                    job->unfinishedDependencies--;
                }
            }
        }

        // If no dependencies, schedule immediately
        if (job->unfinishedDependencies == 0) {
            ScheduleJob(handle);
        }

        m_pendingJobs++;
        return handle;
    }

    void JobSystem::WaitForJob(JobHandle job)
    {
        if (job == 0) return;

        // Simple spin-wait with small sleep to avoid busy-loop
        while (!IsJobFinished(job)) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }

    void JobSystem::WaitAll()
    {
        while (m_pendingJobs > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    bool JobSystem::IsJobFinished(JobHandle job) const
    {
        std::lock_guard<std::mutex> lock(m_jobMapMutex);
        auto it = m_jobs.find(job);
        if (it != m_jobs.end()) {
            return it->second->finished.load();
        }
        // If job not found, consider it finished (maybe cleaned up)
        return true;
    }

    uint32_t JobSystem::GetPendingJobCount() const
    {
        return m_pendingJobs.load();
    }

    void JobSystem::WorkerThread()
    {
        while (!m_stop) {
            JobHandle jobHandle = 0;
            if (m_readyQueue.Pop(jobHandle)) {
                // Execute the job
                InternalJob* job = nullptr;
                {
                    std::lock_guard<std::mutex> lock(m_jobMapMutex);
                    auto it = m_jobs.find(jobHandle);
                    if (it != m_jobs.end()) {
                        job = it->second.get();
                    }
                }

                if (job) {
                    // Mark as started (optional)
                    job->started = true;

                    // Execute job function
                    if (job->function) {
                        job->function(jobHandle, job->userData);
                    }

                    // Mark as finished
                    job->finished = true;

                    // Decrement pending count
                    m_pendingJobs--;

                    // Schedule dependents
                    std::lock_guard<std::mutex> lock(m_jobMapMutex);
                    for (JobHandle dep : job->dependents) {
                        auto it = m_jobs.find(dep);
                        if (it != m_jobs.end()) {
                            int remaining = --(it->second->unfinishedDependencies);
                            if (remaining == 0) {
                                // Job is ready to run
                                ScheduleJob(dep);
                            }
                        }
                    }
                }
            } else {
                // No jobs, yield a bit to avoid busy loop
                std::this_thread::sleep_for(std::chrono::microseconds(500));
            }
        }
    }

    void JobSystem::ScheduleJob(JobHandle job)
    {
        m_readyQueue.Push(job);
    }

    JobHandle JobSystem::GenerateJobId()
    {
        return m_nextJobId.fetch_add(1);
    }

    void JobSystem::CleanupFinishedJobs()
    {
        // Optional: remove finished jobs from map to free memory.
        // For simplicity, we don't automatically clean up; jobs remain until system shutdown.
        // Could be called periodically if needed.
    }

} // namespace USE