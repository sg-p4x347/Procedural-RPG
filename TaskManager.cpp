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

void TaskManager::RunSynchronous(Task && task)
{
	m_waitingSync = true;
	m_active++;
	// block until there are no dependencies 
	std::unique_lock<std::mutex> mlock(m_syncLock);
	m_syncPeek.wait(mlock, [&] {
		return !HasDependendency(task);
	});
	
	
	// Register dependencies
	m_readDependencies = task.ReadDependencies;
	m_writeDependencies = task.WriteDependencies;
	m_queryDependencies = task.QueryMask;
	task.Callback();
	// Unregister dependencies
	m_readDependencies = 0;
	m_writeDependencies = 0;
	m_queryDependencies = 0;
	m_waitingSync = false;
	m_active--;
	
	m_peekCondition.notify_one();
}

void TaskManager::Peek(WorkerThread & thread)
{
	if (!m_waitingSync) {
		m_mutex.lock();
		if (!m_queue.empty()) {
			for (auto it = m_queue.begin(); it != m_queue.end(); it++) {
				Task next = *it;
				// Find any dependencies with this task
				if (!HasDependendency(next)) {
					// no dependencies found
					m_queue.erase(it);
					m_mutex.unlock();
					thread.Run(next);
					m_canPeek = true;
					// try to run another async task
					m_peekCondition.notify_one();
					return;
				}
			}
		}
		m_canPeek = false;
		m_mutex.unlock();
	}
	else {
		// try to run the waiting synchronous task
		m_syncPeek.notify_one();
	}
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

TaskManager::TaskManager() : m_active(0), m_canPeek(true), m_waitingSync(false)
{
	m_threadCount = std::thread::hardware_concurrency() * 8;
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
bool TaskManager::HasDependendency(Task & task)
{
	for (auto & worker : m_threads) {
		if (worker->Active) {
			world::MaskType intersect = worker->QueryDependencies & task.QueryMask;
			if (
				(
				(intersect == worker->QueryDependencies)
					||
					(intersect == task.QueryMask)
					)
				&&
				(
					worker->ReadDependencies & task.WriteDependencies
					||
					worker->WriteDependencies & task.ReadDependencies
					||
					worker->WriteDependencies & task.WriteDependencies
					)
				) {
				// Cannot run this task due to one or more dependencies
				return true;
			}
		}
	}
	// check synchronous task
	world::MaskType intersect = m_queryDependencies & task.QueryMask;
	if (
		(
		(intersect == m_queryDependencies)
			||
			(intersect == task.QueryMask)
			)
		&&
		(
			m_readDependencies & task.WriteDependencies
			||
			m_writeDependencies & task.ReadDependencies
			||
			m_writeDependencies & task.WriteDependencies
			)
		) {
		// Cannot run this task due to one or more dependencies
		return true;
	}
	return false;
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
