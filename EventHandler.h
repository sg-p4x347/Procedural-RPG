#pragma once
#include "pch.h"
template <typename ... Signature>
struct GlobalEventHandler
{
	GlobalEventHandler() {}
	GlobalEventHandler(std::function<void(Signature...)> handler, unsigned int version, bool isStatic) : Handler(handler), Version(version), IsStatic(isStatic) {

	}
	std::function<void(Signature...)> Handler;
	unsigned int Version;
	bool IsStatic;
};

