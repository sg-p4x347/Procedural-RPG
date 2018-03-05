#pragma once
#include "WorkerThread.h"
#include "Task.h"

class TaskManager
{
public:
	static TaskManager & Get();
	void Push(Task task);
private:
	TaskManager();
	~TaskManager();
	
private:
	//----------------------------------------------------------------
	// Threads
	static const UINT m_threadCount = 4;
	vector<shared_ptr<WorkerThread>> m_threads;
	recursive_mutex m_mutex;
	//----------------------------------------------------------------
	// Tasks
	std::queue<Task> m_queue;
	// Tries to run as many tasks as possible with the current dependency graph
	void Peek();
};

