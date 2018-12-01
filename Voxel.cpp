#include "pch.h"
#include "Utility.h"
#include "Voxel.h"
#include "VoxelGrid.h"

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

DirectX::BoundingBox Voxel::Bounds()
{
	return DirectX::BoundingBox(GetPosition(),Vector3::One);
}

Voxel::Ordinal Voxel::GetX()
{
	return m_x;
}

Voxel::Ordinal Voxel::GetY()
{
	return m_y;
}

Voxel::Ordinal Voxel::GetZ()
{
	return m_z;
}

Voxel::Voxel(Voxel::Ordinal x, Voxel::Ordinal y, Voxel::Ordinal z) : m_x(x), m_y(y), m_z(z)
{
}

void Voxel::Import(std::istream & ifs)
{
	
}

void Voxel::Export(std::ostream & ofs)
{
}
