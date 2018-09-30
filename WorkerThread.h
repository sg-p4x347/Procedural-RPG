#pragma once
#include "Component.h"
#include "Task.h"
#include <queue>
#include <atomic>
class TaskManager;
class WorkerThread
{
public:
	WorkerThread(TaskManager * taskManager);
	~WorkerThread();
	void Stop();
	void Run(Task & task);
	std::atomic_ulong ReadDependencies;
	std::atomic_ulong WriteDependencies;
	std::atomic_ulong QueryDependencies;

	std::atomic_bool Active;
private:
	TaskManager * m_taskManager;
	std::thread m_thread;
	std::atomic_bool m_run;
};

