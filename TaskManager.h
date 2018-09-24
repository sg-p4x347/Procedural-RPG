#pragma once
#include "WorkerThread.h"
#include "Task.h"

class TaskManager
{
public:
	static TaskManager & Get();
	void Push(Task && task);
private:
	TaskManager();
	~TaskManager();
	
private:
	//----------------------------------------------------------------
	// Threads
	static const uint32_t m_threadCount = 4;
	std::vector<std::shared_ptr<WorkerThread>> m_threads;
	std::recursive_mutex m_mutex;
	//----------------------------------------------------------------
	// Tasks
	std::list<Task> m_queue;
	// Tries to run as many tasks as possible with the current dependency graph
	void Peek();
};

