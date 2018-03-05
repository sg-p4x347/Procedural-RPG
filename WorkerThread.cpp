#include "pch.h"
#include "WorkerThread.h"


WorkerThread::WorkerThread() : Active(false), ReadDependencies(0), WriteDependencies(0)
{
}


void WorkerThread::Run(Task & task, std::function<void()> && taskFinished)
{
	std::thread([=] {
		// Declare this thread as active
		Active = true;
		// Register dependencies
		ReadDependencies = task.ReadDependencies;
		WriteDependencies = task.WriteDependencies;

		// Execute this task
		task.Callback();

		// Unregister dependencies
		ReadDependencies = 0;
		WriteDependencies = 0;
		// Declare this thread as inactive
		Active = false;
		// Finished callback
		taskFinished();
	}).detach();
}
