#include "pch.h"
#include "TaskManager.h"
#include <queue>

TaskManager & TaskManager::Get()
{
	//----------------------------------------------------------------
	// Singleton
	static TaskManager instance;
	return instance;
}

void TaskManager::Push(Task & task)
{
	// Find a thread that shares a dependency with this task
	WorkerThread * thread = FindThreadWith(task.Dependencies);
	if (thread) {
		thread->Push(task);
	} else {
		// make a new thread
		auto newThread = std::make_shared<WorkerThread>(m_threads);
		m_threads.push_back(newThread);
		newThread->Push(task);
		newThread->Run();
	}
	m_queue.push(task);


}

TaskManager::TaskManager()
{
}


TaskManager::~TaskManager()
{
}

WorkerThread * TaskManager::FindThreadWith(std::set<shared_ptr<Components::Component>> dependencies)
{
	for (auto & thread : m_threads) {
		for (auto & dependency : dependencies) {
			if (thread->Dependencies.find(dependency) != thread->Dependencies.end()) return thread.get();
		}
	}
	// None found
	return nullptr;
}
