#pragma once
#include "EntityTypedefs.h"
class Task
{
public:
	Task(function<void(void)> callback, world::MaskType queryMask = 0, world::MaskType readDependencies = 0, world::MaskType writeDependencies = 0);
	~Task();
	function<void(void)> Callback;
	world::MaskType QueryMask;
	world::MaskType ReadDependencies;
	world::MaskType WriteDependencies;
};

