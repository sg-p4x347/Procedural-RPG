#pragma once
#include "Component.h"
#include "Task.h"
#include <queue>
#include <atomic>
class WorkerThread
{
public:
	WorkerThread();
	void Run(Task & task,std::function<void()> && taskFinished);
	std::atomic_ulong ReadDependencies;
	std::atomic_ulong WriteDependencies;
	std::atomic_bool Active;
};

