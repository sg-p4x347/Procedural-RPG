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
	m_canPeek = true;
	m_peekCondition.notify_one();
}

void TaskManager::Peek(WorkerThread & thread)
{
	m_mutex.lock();
	if (!m_queue.empty()) {
		if (m_queue.size() > 1) {
			auto test = 0;
		}
		for (auto it = m_queue.begin(); it != m_queue.end(); it++) {
			Task next = *it;
			// Find any dependencies with this task
			for (auto & worker : m_threads) {
				if (worker->Active) {
					world::MaskType intersect = worker->QueryDependencies & next.QueryMask;
					if (
						(
							(intersect == worker->QueryDependencies)
							||
							(intersect == next.QueryMask)
						)
						&&
						(
							worker->ReadDependencies & next.WriteDependencies
							||
							worker->WriteDependencies & next.ReadDependencies
							||
							worker->WriteDependencies & next.WriteDependencies
						)
					) {
						// Cannot run this task due to one or more dependencies
						goto tryNext;
					}
				}
			}
			// no dependencies found
			m_queue.erase(it);
			m_mutex.unlock();
			thread.Run(next);
			m_canPeek = true;
			m_peekCondition.notify_one();
			return;
			tryNext :;
		}
		m_canPeek = false;
	}
	else {
		m_canPeek = false;
	}
	m_mutex.unlock();
}

void TaskManager::WaitForAll()
{
	while (m_active);
}

bool TaskManager::QueueEmpty()
{
	//lock_guard<std::recursive_mutex> guard(m_mutex);
	return m_queue.empty();
}

TaskManager::TaskManager() : m_active(0)
{
	m_threadCount = std::thread::hardware_concurrency() * 32;
	// Initialize the threads
	for (int i = 0; i < m_threadCount; i++) {
		m_threads.push_back(std::make_shared<WorkerThread>(this));
	}
}


TaskManager::~TaskManager()
{
	// Stop the threads
	for (int i = 0; i < m_threadCount; i++) {
		m_threads[i]->Stop();
	}
	m_peekCondition.notify_all();
}
// Tries to run every item in the queue
//void TaskManager::Peek()
//{
//	//m_mutex.lock();
//
//	//if (!m_queue.empty()) {
//	//	begin:
//	//	for (auto it = m_queue.begin(); it != m_queue.end(); it++) {
//	//		Task next = *it;
//	//		// Find any dependencies with this task
//	//		std::shared_ptr<WorkerThread> freeThread;
//	//		for (auto & worker : m_threads) {
//	//			if (worker->Active) {
//	//				if (worker->ReadDependencies & next.WriteDependencies
//	//					|
//	//					worker->WriteDependencies & next.ReadDependencies) {
//	//					// Cannot run this task due to one or more dependencies
//	//					goto end;
//	//				}
//	//			}
//	//			else if (!freeThread) {
//	//				// First free thread
//	//				freeThread = worker;
//	//			}
//	//		}
//	//		// no dependencies found, so try to run on an open thread
//	//		if (freeThread) {
//	//			m_queue.erase(it);
//	//			freeThread->Run(next, [=] {
//	//				// When task is finished, attempt to run another task
//	//				Peek();
//	//			});
//	//			// start over from the top of the list
//	//			goto begin;
//	//		}
//	//		end:;
//	//	}
//	//}
//	//m_mutex.unlock();
//}

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
