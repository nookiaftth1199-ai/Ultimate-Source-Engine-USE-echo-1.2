// ============================================================
// Ultimate Source Engine - Job System
// ============================================================
//
// Provides a job-based parallel task system with dependencies.
// Jobs are lightweight functions that can be scheduled to run
// on worker threads. Dependencies ensure that a job runs only
// after its prerequisites have completed.
// ============================================================

#pragma once

#include <functional>
#include <atomic>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <memory>

namespace USE {

    // Forward declarations
    class JobSystem;

    // -----------------------------------------------------------------
    // Job handle (opaque ID)
    // -----------------------------------------------------------------
    using JobHandle = uint64_t;

    // -----------------------------------------------------------------
    // Job function type (takes a JobHandle for the job itself and user data)
    // -----------------------------------------------------------------
    using JobFunction = std::function<void(JobHandle, void*)>;

    // -----------------------------------------------------------------
    // Job description (public interface)
    // -----------------------------------------------------------------
    struct JobDesc {
        JobFunction   function;      // The job's work function
        void*         userData;      // User data passed to the function
        const char*   name;          // Optional name for debugging
        JobHandle*    dependencies;  // Array of job handles this job depends on
        int           numDependencies; // Number of dependencies

        JobDesc()
            : function(nullptr)
            , userData(nullptr)
            , name(nullptr)
            , dependencies(nullptr)
            , numDependencies(0)
        {}
    };

    // -----------------------------------------------------------------
    // Main Job System class
    // -----------------------------------------------------------------
    class JobSystem {
    public:
        JobSystem();
        ~JobSystem();

        // Initialize with a given number of worker threads (0 = auto-detect)
        bool Initialize(uint32_t numThreads = 0);
        void Shutdown();

        // Create and schedule a job. Returns a handle.
        JobHandle CreateJob(const JobDesc& desc);

        // Wait for a specific job to complete.
        void WaitForJob(JobHandle job);

        // Wait for all jobs to complete (including any jobs created by jobs).
        void WaitAll();

        // Check if a job is finished.
        bool IsJobFinished(JobHandle job) const;

        // Get the number of pending jobs (not yet started).
        uint32_t GetPendingJobCount() const;

        // Get the number of worker threads.
        uint32_t GetWorkerThreadCount() const { return m_numThreads; }

    private:
        // Internal job structure
        struct InternalJob {
            JobHandle       id;                 // Unique ID
            JobFunction     function;           // Work function
            void*           userData;           // User data
            const char*     name;               // Debug name
            std::atomic<int> unfinishedDependencies; // Counter of dependencies not yet done
            std::vector<JobHandle> dependents;  // Jobs that depend on this one
            std::atomic<bool> finished;         // True when job is done
            std::atomic<bool> started;          // True when job has been queued for execution

            InternalJob()
                : id(0)
                , function(nullptr)
                , userData(nullptr)
                , name(nullptr)
                , unfinishedDependencies(0)
                , finished(false)
                , started(false)
            {}
        };

        // Thread-safe queue of ready jobs
        class ReadyQueue {
        public:
            void Push(JobHandle job);
            bool Pop(JobHandle& job);
            size_t Size() const;
        private:
            mutable std::mutex m_mutex;
            std::queue<JobHandle> m_queue;
        };

        // Worker thread function
        void WorkerThread();

        // Schedule a job (internal) – adds to ready queue if dependencies are satisfied.
        void ScheduleJob(JobHandle job);

        // Get internal job by handle
        InternalJob* GetJob(JobHandle handle);
        const InternalJob* GetJob(JobHandle handle) const;

        // Generate a new job handle
        JobHandle GenerateJobId();

        // Clean up finished jobs (optional, could be done periodically)
        void CleanupFinishedJobs();

    private:
        std::vector<std::thread>    m_workers;           // Worker threads
        std::atomic<bool>           m_stop;              // Shutdown flag
        std::atomic<uint32_t>       m_numThreads;        // Number of worker threads

        // Job storage (mapping handle -> InternalJob)
        mutable std::mutex           m_jobMapMutex;
        std::unordered_map<JobHandle, std::unique_ptr<InternalJob>> m_jobs;
        std::atomic<JobHandle>       m_nextJobId;         // Next unique job ID

        // Ready queue for jobs that can be executed now
        ReadyQueue                   m_readyQueue;

        // Synchronization for waiting
        mutable std::mutex           m_waitMutex;
        std::condition_variable      m_waitCondition;
        std::atomic<uint32_t>        m_pendingJobs;       // Number of jobs not yet finished

        // For waiting on specific jobs
        struct WaitNode {
            JobHandle job;
            bool finished;
            std::condition_variable cv;
        };
        // Not implemented fully; could use a per-job condition or just busy-wait.
        // We'll keep simple: WaitForJob will busy-spin or use a condition on job->finished.
    };

} // namespace USE