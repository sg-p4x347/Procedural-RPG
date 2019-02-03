#include "pch.h"
#include "WorkerThread.h"
#include "TaskManager.h"

WorkerThread::WorkerThread(TaskManager * taskManager) : m_taskManager(taskManager), Active(false), ReadDependencies(0), WriteDependencies(0), m_run(true), m_thread(std::thread([&] {
	while (m_run) {
		std::unique_lock<std::mutex> mlock(m_taskManager->m_peekMutex);
		m_taskManager->m_peekCondition.wait(mlock, [&] {
			return m_taskManager->m_canPeek || !m_run;
		});
		mlock.unlock();
		if (!m_run) return;
		m_taskManager->Peek(*this);
	}
}))
{
}

WorkerThread::~WorkerThread()
{
	m_thread.join();
}

void WorkerThread::Stop()
{
	m_run = false;
}


void WorkerThread::Run(Task & task)
{
	// Declare this thread as active
	m_taskManager->m_active++;
	Active = true;
	// Register dependencies
	ReadDependencies = task.ReadDependencies;
	WriteDependencies = task.WriteDependencies;
	QueryDependencies = task.QueryMask;

	// Execute this task
	task.Callback();

	// Unregister dependencies
	ReadDependencies = 0;
	WriteDependencies = 0;
	QueryDependencies = 0;
	// Declare this thread as inactive
	Active = false;
	m_taskManager->m_active--;
}
