#pragma once
#include "pch.h"
template <typename ... Signature>
struct GlobalEventHandler
{
	GlobalEventHandler() {}
	GlobalEventHandler(std::function<void(Signature...)> handler, unsigned int version) : Handler(handler), Version(version) {

	}
	std::function<void(Signature...)> Handler;
	unsigned int Version;
};

