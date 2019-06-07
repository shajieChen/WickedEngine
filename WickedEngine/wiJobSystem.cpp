#include "wiJobSystem.h"
#include "wiSpinLock.h"
#include "wiBackLog.h"
#include "wiContainers.h"

#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <sstream>
#include <algorithm>
#include <array>
#include <vector>

namespace wiJobSystem
{
	uint32_t numThreads = 0;
	std::condition_variable wakeCondition;
	std::mutex wakeMutex;

	struct wiJobQueue
	{
		wiContainers::ThreadSafeRingBuffer<std::function<void()>, 256> jobPool;
		uint64_t currentLabel = 0;
		std::atomic<uint64_t> finishedLabel = 0;
		uint64_t threadMask = ~0;
	};
	std::array<wiJobQueue, 8> jobQueues;
	std::vector<uint8_t> free_queueIDs = { 1,2,3,4,5,6,7 }; // if you grab a free queue ID from here...
	std::vector<uint8_t> active_queueIDs = { 0 }; // ...then put it here so the queue will be used by workers
	inline wiJobQueue& get_job_queue(uint8_t queueID)
	{
		return jobQueues.at(queueID);
	}

	void Initialize()
	{
		// Retrieve the number of hardware threads in this system:
		auto numCores = std::thread::hardware_concurrency();

		// Calculate the actual number of worker threads we want:
		numThreads = std::max(1u, numCores - 1);

		for (uint32_t threadID = 0; threadID < numThreads; ++threadID)
		{
			const uint64_t threadMask = 1ull << threadID;

			std::thread worker([threadMask] {

				std::function<void()> job;

				while (true)
				{
					bool got_job = false;
					for (auto& queueID : active_queueIDs)
					{
						wiJobQueue& jobQueue = get_job_queue(queueID);

						if ((jobQueue.threadMask & threadMask) && jobQueue.jobPool.pop_front(job))
						{
							job(); // execute job
							jobQueue.finishedLabel.fetch_add(1); // update worker label state
							got_job = true;
							break;
						}
					}

					if (!got_job)
					{
						// no job, put thread to sleep
						std::unique_lock<std::mutex> lock(wakeMutex);
						wakeCondition.wait(lock);
					}
				}

			});

#ifdef _WIN32
			// Do Windows-specific thread setup:
			HANDLE handle = (HANDLE)worker.native_handle();

			// Put each thread on to dedicated core
			DWORD_PTR affinityMask = threadMask; 
			DWORD_PTR affinity_result = SetThreadAffinityMask(handle, affinityMask);
			assert(affinity_result > 0);

			//// Increase thread priority:
			//BOOL priority_result = SetThreadPriority(handle, THREAD_PRIORITY_HIGHEST);
			//assert(priority_result != 0);

			// Name the thread:
			std::wstringstream wss;
			wss << "wiJobSystem_" << threadID;
			HRESULT hr = SetThreadDescription(handle, wss.str().c_str());
			assert(SUCCEEDED(hr));
#endif // _WIN32
			
			worker.detach();
		}

		std::stringstream ss("");
		ss << "wiJobSystem Initialized with [" << numCores << " cores] [" << numThreads << " threads]";
		wiBackLog::post(ss.str().c_str());
	}

	// This little function will not let the system to be deadlocked while the main thread is waiting for something
	inline void poll()
	{
		wakeCondition.notify_one(); // wake one worker thread
		std::this_thread::yield(); // allow this thread to be rescheduled
	}

	uint32_t GetThreadCount()
	{
		return numThreads;
	}

	void Execute(const std::function<void()>& job, uint8_t queueID)
	{
		wiJobQueue& jobQueue = get_job_queue(queueID);

		// The main thread label state is updated:
		jobQueue.currentLabel += 1;

		// Try to push a new job until it is pushed successfully:
		while (!jobQueue.jobPool.push_back(job)) { poll(); }

		wakeCondition.notify_one(); // wake one thread
	}

	void Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(wiJobDispatchArgs)>& job, uint8_t queueID)
	{
		if (jobCount == 0 || groupSize == 0)
		{
			return;
		}

		wiJobQueue& jobQueue = get_job_queue(queueID);

		// Calculate the amount of job groups to dispatch (overestimate, or "ceil"):
		const uint32_t groupCount = (jobCount + groupSize - 1) / groupSize;

		// The main thread label state is updated:
		jobQueue.currentLabel += groupCount;

		for (uint32_t groupIndex = 0; groupIndex < groupCount; ++groupIndex)
		{
			// For each group, generate one real job:
			const auto& jobGroup = [jobCount, groupSize, job, groupIndex]() {

				// Calculate the current group's offset into the jobs:
				const uint32_t groupJobOffset = groupIndex * groupSize;
				const uint32_t groupJobEnd = std::min(groupJobOffset + groupSize, jobCount);

				wiJobDispatchArgs args;
				args.groupIndex = groupIndex;

				// Inside the group, loop through all job indices and execute job for each index:
				for (uint32_t i = groupJobOffset; i < groupJobEnd; ++i)
				{
					args.jobIndex = i;
					job(args);
				}
			};

			// Try to push a new job until it is pushed successfully:
			while (!jobQueue.jobPool.push_back(jobGroup)) { poll(); }

			wakeCondition.notify_one(); // wake one thread
		}


	}

	bool IsBusy()
	{
		for (auto& queueID : active_queueIDs)
		{
			wiJobQueue& jobQueue = get_job_queue(queueID);

			// Whenever the queue target label is not reached by the workers, it indicates that some worker is still alive
			if (jobQueue.finishedLabel.load() < jobQueue.currentLabel)
			{
				return true;
			}
		}
		return false;
	}
	void WaitIdle()
	{
		// Wait until all queues are empty
		while (IsBusy()) { poll(); }
	}


	uint64_t Barrier(uint8_t queueID)
	{
		wiJobQueue& jobQueue = get_job_queue(queueID);

		// This function essentially puts a blocking job on all worker threads to synchronize them.

		// All work needs to be completed up to this point:
		const uint64_t barrier = jobQueue.currentLabel;

		// The job will simply block until the barrier was reached by all workers:
		const auto& job = [barrier, queueID] {
			while (get_job_queue(queueID).finishedLabel.load() < barrier) { std::this_thread::yield(); };
		};

		// Worker threads will need to be blocked:
		const uint32_t numJobs = numThreads;

		// Must update the queue state because these are jobs too:
		jobQueue.currentLabel += numJobs;

		// Put numThreads amount of blocking jobs to the queue so that every thread will block:
		for (uint32_t jobID = 0; jobID < numJobs; ++jobID)
		{
			while (!jobQueue.jobPool.push_back(job)) { poll(); }
		}

		// Wake all threads in case some were already sleeping:
		wakeCondition.notify_all();

		// Return the barrier so that application can wait on it or check for completion:
		return barrier;
	}
	bool IsBarrierReached(uint64_t barrier, uint8_t queueID)
	{
		wiJobQueue& jobQueue = get_job_queue(queueID);

		// This works like IsBusy(), but checks for a specific execution state point that was created with Barrier():
		return jobQueue.finishedLabel.load() >= barrier;
	}
	void DrainBarrier(uint64_t barrier, uint8_t queueID)
	{
		// This works like WaitIdle(), but only blocks until a barrier was reached on the specified queue, then unblocks.
		while (!IsBarrierReached(barrier, queueID)) { poll(); }
	}


	uint8_t Create_JobQueue(uint64_t threadMask)
	{
		if (free_queueIDs.empty())
		{
			return 0;
		}

		WaitIdle(); // active_queueIDs has contention with worker threads
		uint8_t queueID = free_queueIDs.back();
		free_queueIDs.pop_back();
		active_queueIDs.push_back(queueID);
		wiJobQueue& jobQueue = get_job_queue(queueID);
		jobQueue.threadMask = threadMask;
		return queueID;
	}
	void Delete_JobQueue(uint8_t queueID)
	{
		if (queueID > 0 && queueID < jobQueues.size())
		{
			WaitIdle(); // active_queueIDs has contention with worker threads
			active_queueIDs.erase(active_queueIDs.begin() + queueID);
			free_queueIDs.push_back(queueID);
		}
	}
}
