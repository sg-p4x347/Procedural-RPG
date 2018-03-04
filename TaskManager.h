#pragma once
#include "WorkerThread.h"
#include "Task.h"

class TaskManager
{
public:
	static TaskManager & Get();
	void Push(Task & task);
private:
	TaskManager();
	~TaskManager();
private:
	//----------------------------------------------------------------
	// Dependencies
	WorkerThread * FindThreadWith(std::set<shared_ptr<Components::Component>> dependencies);
	//----------------------------------------------------------------
	// Threads
	vector<shared_ptr<WorkerThread>> m_threads;
	//----------------------------------------------------------------
	// Tasks
	std::queue<Task> m_queue;
};

