#include "pch.h"
#include "Task.h"


Task::Task(function<void(void)> callback, unsigned long read, unsigned long write) : Callback(callback),ReadDependencies(read), WriteDependencies(write)
{
}

Task::~Task()
{
}
