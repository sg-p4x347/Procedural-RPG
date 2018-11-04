#pragma once
#include "WorkerThread.h"
#include "Task.h"

class TaskManager
{
public:
	static TaskManager & Get();
	void Push(Task && task);
	void RunSynchronous(Task && task);
	// tries to run a task on the given thread
	void Peek(WorkerThread & thread);
	void WaitForAll();
	std::condition_variable m_peekCondition;
	std::condition_variable m_syncPeek;
	std::mutex m_peekMutex;
	std::atomic_bool m_waitingSync;
	std::atomic_bool m_canPeek;
	std::atomic_int m_active;
	bool QueueEmpty();
private:
	TaskManager();
	~TaskManager();
	
private:
	//----------------------------------------------------------------
	// Threads
	uint32_t m_threadCount;
	std::vector<std::shared_ptr<WorkerThread>> m_threads;
	std::mutex m_mutex;
	
	
	//----------------------------------------------------------------
	// Tasks
	std::list<Task> m_queue;
	bool HasDependendency(Task & task);
};

