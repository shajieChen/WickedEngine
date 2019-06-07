#include "LoadingScreen.h"
#include "MainComponent.h"
#include "wiJobSystem.h"

using namespace std;

LoadingScreen::LoadingScreen() : RenderPath2D()
{
	// Create a separate job queue for the loading screen so that it can run independently from other job system queues.
	jobQueueID = wiJobSystem::Create_JobQueue();
}
LoadingScreen::~LoadingScreen()
{
	wiJobSystem::Delete_JobQueue(jobQueueID);
}

bool LoadingScreen::isBusy()
{
	return !wiJobSystem::IsBarrierReached(barrier, jobQueueID);
}

void LoadingScreen::addLoadingTask(const function<void()>& task)
{
	if (task != nullptr)
	{
		tasks.push_back(task);
	}
}

void LoadingScreen::addLoadingComponent(RenderPath* component, MainComponent* main, float fadeSeconds, const wiColor& fadeColor)
{
	addLoadingTask([=] {
		component->Load();
	});
	onFinished([=] {
		main->ActivatePath(component, fadeSeconds, fadeColor);
	});
}

void LoadingScreen::onFinished(function<void()> finishFunction)
{
	if (finishFunction != nullptr)
	{
		finish = finishFunction;
	}
}

void LoadingScreen::Start()
{
	barrier = ~0; // mark as always working
	for (auto& task : tasks)
	{
		wiJobSystem::Execute(task, jobQueueID); // add loading tasks to job queue
	}
	wiJobSystem::Barrier(jobQueueID); // tasks must complete before moving on
	wiJobSystem::Execute(finish, jobQueueID); // add finish task to job queue
	barrier = wiJobSystem::Barrier(jobQueueID); // mark this as the end of loading screen
	// all of the above are async calls, this does not block!

	RenderPath2D::Start();
}

void LoadingScreen::Stop()
{
	wiJobSystem::DrainBarrier(barrier, jobQueueID); // drain all work here, just in case it wasn't completed yet
	tasks.clear();
	finish = nullptr;

	RenderPath2D::Stop();
}

