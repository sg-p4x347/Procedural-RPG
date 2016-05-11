#pragma once
#include "pch.h"

using namespace DirectX;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
//struct SimpleVertex
//{
//	XMFLOAT3 position;
//	XMFLOAT2 texture;
//};
struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};