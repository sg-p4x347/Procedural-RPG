#include "pch.h"
#include "TaskManager.h"

TaskManager & TaskManager::Get()
{
	//----------------------------------------------------------------
	// Singleton
	static TaskManager instance;
	return instance;
}

void TaskManager::Push(Task && task)
{
	m_mutex.lock();
	m_queue.push_back(std::move(task));
	m_mutex.unlock();
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
// Tries to run every item in the queue
void TaskManager::Peek()
{
	m_mutex.lock();

	if (!m_queue.empty()) {
		begin:
		for (auto it = m_queue.begin(); it != m_queue.end(); it++) {
			Task next = *it;
			// Find any dependencies with this task
			std::shared_ptr<WorkerThread> freeThread;
			for (auto & worker : m_threads) {
				if (worker->Active) {
					if (worker->ReadDependencies & next.WriteDependencies
						|
						worker->WriteDependencies & next.ReadDependencies) {
						// Cannot run this task due to one or more dependencies
						goto end;
					}
				}
				else if (!freeThread) {
					// First free thread
					freeThread = worker;
				}
			}
			// no dependencies found, so try to run on an open thread
			if (freeThread) {
				m_queue.erase(it);
				freeThread->Run(next, [=] {
					// When task is finished, attempt to run another task
					Peek();
				});
				// start over from the top of the list
				goto begin;
			}
			end:;
		}
	}
	m_mutex.unlock();
}

// Tries to run only the next item in the queue
//void TaskManager::Peek()
//{
//	m_mutex.lock();
//	if (!m_queue.empty()) {
//		Task next = m_queue.front();
//		// Find any dependencies with this task
//		std::shared_ptr<WorkerThread> freeThread;
//		for (auto & worker : m_threads) {
//			if (worker->Active) {
//				if (worker->ReadDependencies & next.WriteDependencies
//					|
//					worker->WriteDependencies & next.ReadDependencies) {
//					// Cannot run this task due to one or more dependencies
//					m_mutex.unlock();
//					return;
//				}
//			}
//			else if (!freeThread) {
//				// First free thread
//				freeThread = worker;
//			}
//		}
//		// no dependencies found, so try to run on an open thread
//		if (freeThread) {
//			m_queue.pop();
//			freeThread->Run(next, [=] {
//				// When task is finished, attempt to run another task
//				Peek();
//			});
//			// Try to run another task recursively
//			Peek();
//		}
//	}
//	m_mutex.unlock();
//}
