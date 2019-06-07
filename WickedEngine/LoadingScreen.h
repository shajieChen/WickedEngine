#pragma once
#include "RenderPath2D.h"
#include "wiColor.h"

#include <functional>

class MainComponent;

class LoadingScreen :
	public RenderPath2D
{
private:
	uint8_t jobQueueID = ~0;
	uint64_t barrier = 0;
	std::vector< std::function<void()> > tasks;
	std::function<void()> finish;
public:
	LoadingScreen();
	virtual ~LoadingScreen();

	//Add a loading task which should be executed
	//use std::bind( YourFunctionPointer )
	void addLoadingTask(const std::function<void()>& task);
	//Helper for loading a whole renderable component
	void addLoadingComponent(RenderPath* component, MainComponent* main, float fadeSeconds = 0, const wiColor& fadeColor = wiColor(0, 0, 0, 255));
	//Set a function that should be called when the loading finishes
	//use std::bind( YourFunctionPointer )
	void onFinished(std::function<void()> finishFunction);
	//See if the loading is currently running
	bool isBusy();

	//Start Executing the tasks and mark the loading as active
	virtual void Start() override;
	//Clear all tasks
	virtual void Stop() override;
};

