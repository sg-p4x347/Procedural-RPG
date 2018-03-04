#include "pch.h"
#include "Task.h"


Task::Task(function<void(void)> callback, unsigned long & dependencies) : Callback(callback),Dependencies(dependencies)
{
}

Task::~Task()
{
}
