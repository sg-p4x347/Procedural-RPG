#include "pch.h"
#include "RegionJobCache.h"

void RegionJobCache::Clear()
{
	opaque.clear();
	alpha.clear();
}

void RegionJobCache::Add(RegionJobCache & other)
{
	opaque.insert(opaque.end(), other.opaque.begin(), other.opaque.end());
	alpha.insert(alpha.end(), other.alpha.begin(), other.alpha.end());
}
