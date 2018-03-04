#pragma once
#include "Component.h"

class Task
{
public:
	Task(function<void(void)> callback, unsigned long & dependencies);
	~Task();
	function<void(void)> Callback;
	unsigned long Dependencies;
private:
	
};

