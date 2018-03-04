#include "pch.h"
#include "WorkerThread.h"


WorkerThread::WorkerThread(vector<shared_ptr<WorkerThread>> & threads) : m_threads(threads)
{
}

void WorkerThread::Push(Task & task)
{
	// Union the dependencies with this thread
	Dependencies |= task.Dependencies;
	m_tasks.push(task);
}

void WorkerThread::Run()
{
	m_thread = std::thread([=] {
		while (!m_tasks.empty()) {
			m_tasks.front().Callback();
			m_tasks.pop();
		}
		// remove this thread
		auto & it = m_threads
		m_threads.erase()
	});
	m_thread.detach();
}

void WorkerThread::UpdateDependencies()
{
	Dependencies = 0;
	for (auto task) {
		Dependencies += task.Dependency;
	}
}
