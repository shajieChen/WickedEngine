#pragma once

#include <functional>

struct wiJobDispatchArgs
{
	uint32_t jobIndex;
	uint32_t groupIndex;
};

namespace wiJobSystem
{
	void Initialize();

	uint32_t GetThreadCount();

	// Add a job to execute asynchronously. Any idle thread will execute this job.
	//	queueID		: can be specified to issue on a specific queue instead of the default
	void Execute(const std::function<void()>& job, uint8_t queueID = 0);

	// Divide a job onto multiple jobs and execute in parallel.
	//	jobCount	: how many jobs to generate for this task.
	//	groupSize	: how many jobs to execute per thread. Jobs inside a group execute serially. It might be worth to increase for small jobs
	//	func		: receives a wiJobDispatchArgs as parameter
	//	queueID		: can be specified to issue on a specific queue instead of the default
	void Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(wiJobDispatchArgs)>& job, uint8_t queueID = 0);

	// Check if any threads are working currently or not
	bool IsBusy();
	// Wait until all threads become idle. The current thread will block until the whole system becomes idle.
	void WaitIdle();


	// Insert a barrier to drain pending jobs. The current thread will not block. Worker threads will block until all prior work to the barrier was finished.
	//	queueID		: can be specified to issue on a specific queue instead of the default
	uint64_t Barrier(uint8_t queueID = 0);
	// Check if a barrier has been reached by all workers or not.
	//	barrier		: the barrier to check; barriers can be created with Barrier()
	//	queueID		: can be specified to issue on a specific queue instead of the default
	bool IsBarrierReached(uint64_t barrier, uint8_t queueID = 0);
	// The current thread will block until all workers reached the barrier. The current thread will take work if there is anything available.
	//	barrier		: the barrier to wait on; barriers can be created with Barrier()
	//	queueID		: can be specified to issue on a specific queue instead of the default
	void DrainBarrier(uint64_t barrier, uint8_t queueID = 0);


	// Returns a new job queue ID if successful, 0 otherwise. You can specify threadMask so that only certain threads will execute this; each set bit will correspond to one system thread that can execute on the queue.
	//	This is not thread safe, you must only call this while all queues are drained!
	uint8_t Create_JobQueue(uint64_t threadMask = ~0);
	// Deletes a job queue if it exists.
	//	This is not thread safe, you must only call this while all queues are drained!
	void Delete_JobQueue(uint8_t queueID);
}
