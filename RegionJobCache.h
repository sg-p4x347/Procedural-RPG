#pragma once
#include "RenderJob.h"
struct RegionJobCache
{
	std::vector<RenderJob> opaque;
	std::vector<RenderJob> alpha;
	void Clear();
};

