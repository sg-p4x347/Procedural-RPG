#include "pch.h"
#include "TaskManager.h"
#include <queue>
#include <mutex>
TaskManager & TaskManager::Get()
{
	//----------------------------------------------------------------
	// Singleton
	static TaskManager instance;
	return instance;
}

void TaskManager::Push(Task task)
{
	m_queue.push(task);
	Peek();
}

TaskManager::TaskManager()
{
	// Initialize the threads
	for (int i = 0; i < m_threadCount; i++) {
		m_threads.push_back(std::make_shared<WorkerThread>());
	}
}


TaskManager::~TaskManager()
{
}

void TaskManager::Peek()
{
	m_mutex.lock();
	if (!m_queue.empty()) {
		Task next = m_queue.front();
		// Find any dependencies with this task
		shared_ptr<WorkerThread> freeThread;
		for (auto & worker : m_threads) {
			if (worker->Active) {
				unsigned long overlap =
					worker->ReadDependencies & next.WriteDependencies
					|
					worker->WriteDependencies & next.ReadDependencies;
				if (overlap) {
					// Cannot run this task due to one or more dependencies
					m_mutex.unlock();
					return;
				}
			}
			else if (!freeThread) {
				// First free thread
				freeThread = worker;
			}
		}
		// no dependencies found, so try to run on an open thread
		if (freeThread) {
			m_queue.pop();
			freeThread->Run(next, [=] {
				// When task is finished, attempt to run another task
				Peek();
			});
			// Try to run another task recursively
			Peek();
		}
	}
	m_mutex.unlock();
}
