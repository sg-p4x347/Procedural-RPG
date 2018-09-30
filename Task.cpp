#include "pch.h"
#include "Task.h"


Task::Task(function<void(void)> callback,world::MaskType queryMask, world::MaskType read, world::MaskType write) : Callback(callback),QueryMask(queryMask), ReadDependencies(read), WriteDependencies(write)
{
}

Task::~Task()
{
}
