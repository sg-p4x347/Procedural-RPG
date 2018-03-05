#pragma once
#include "Component.h"

class Task
{
public:
	Task(function<void(void)> callback, unsigned long read = 0, unsigned long write = 0);
	~Task();
	function<void(void)> Callback;
	unsigned long ReadDependencies;
	unsigned long WriteDependencies;
};

