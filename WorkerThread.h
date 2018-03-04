#pragma once
#include "Component.h"
#include "Task.h"
#include <queue>
class WorkerThread
{
public:
	WorkerThread(vector<shared_ptr<WorkerThread>> & threads);
	void Push(Task & task);
	void Run();
	void UpdateDependencies();
	unsigned long Dependencies;
	vector<shared_ptr<WorkerThread>> & m_threads;
private:
	thread m_thread;
	queue<Task> m_tasks;
};

