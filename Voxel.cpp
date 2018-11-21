#include "pch.h"
#include "Utility.h"
#include "Voxel.h"

using namespace DirectX;
using namespace Utility;

Voxel::Voxel()
{
}


Voxel::~Voxel()
{
}

Vector3 Voxel::GetPosition()
{
	return Vector3(m_x, m_y, m_z);
}

int Voxel::GetX()
{
	return m_x;
}

int Voxel::GetY()
{
	return m_y;
}

int Voxel::GetZ()
{
	return m_z;
}

Voxel::Voxel(int x, int y, int z) : m_x(x), m_y(y), m_z(z)
{
}
