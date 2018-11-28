#include "pch.h"
#include "VoxelGrid.h"
#include "CollisionUtil.h"
VoxelGrid::VoxelGrid()
{
}

VoxelGrid::VoxelGrid(Voxel::Ordinal xSize, Voxel::Ordinal ySize, Voxel::Ordinal zSize)


VoxelGrid::~VoxelGrid()
{
}

const std::vector<std::vector<std::shared_ptr<Voxel>>> & VoxelGrid::operator[](const int index)
{
	return m_voxels[index];
}

std::set<std::shared_ptr<Voxel>> VoxelGrid::GetIntersection(DirectX::BoundingFrustum & frustum)
{
	std::set<std::shared_ptr<Voxel>> intersection;
	return intersection;
}

std::set<std::shared_ptr<Voxel>> VoxelGrid::GetIntersection(geometry::ConvexHull & convexHull)
{
	std::set<std::shared_ptr<Voxel>> broadPhase;
	DirectX::BoundingBox hullBounds = convexHull.Bounds();
	for (auto & voxel : *this) {
		if (voxel && hullBounds.Intersects(voxel->Bounds())) {
			broadPhase.insert(voxel);
		}
	}
	std::set<std::shared_ptr<Voxel>> narrowPhase;
	for (auto & voxel : broadPhase) {
		CollisionUtil::GjkIntersection intersection;
		auto voxelHull = geometry::ConvexHull(voxel->Bounds());
		if (CollisionUtil::GJK(convexHull.vertices, voxelHull.vertices, intersection)) {
			narrowPhase.insert(voxel);
		}
	}
	return narrowPhase;
}

VoxelGridIterator VoxelGrid::begin()
{
	return VoxelGridIterator(*this);
}

VoxelGridIterator VoxelGrid::end()
{
	return VoxelGridIterator(*this,true);
}

void VoxelGrid::Import(std::istream & ifs)
{
	DeSerialize(m_xSize, ifs);
	DeSerialize(m_ySize, ifs);
	DeSerialize(m_zSize, ifs);
	for (Voxel::Ordinal x = 0; x < m_xSize; x++) {
		for (Voxel::Ordinal y = 0; y < m_ySize; y++) {
			for (Voxel::Ordinal z = 0; z < m_zSize; z++) {
				
			}
		}
	}
}

void VoxelGrid::Export(std::ostream & ofs)
{
	Serialize(m_xSize, ofs);
	Serialize(m_ySize, ofs);
	Serialize(m_zSize, ofs);
	for (auto & voxel : *this) {
		voxel->Export(ofs);
	}
}

void VoxelGrid::Set(Voxel && voxel, Voxel::Ordinal x, Voxel::Ordinal y, Voxel::Ordinal z)
{
	assert(!Bounded(x, y, z));

	voxel.m_x = x;
	voxel.m_y = y;
	voxel.m_z = z;
	m_voxels[x][y][z] = std::make_shared<Voxel>(std::move(voxel));
}

bool VoxelGrid::Bounded(Voxel::Ordinal x, Voxel::Ordinal y, Voxel::Ordinal z)
{
	return
		x < m_xSize
		&&
		y < m_ySize
		&&
		z < m_zSize;

}

const Voxel::Ordinal & VoxelGrid::GetXsize()
{
	return m_xSize;
}

const Voxel::Ordinal & VoxelGrid::GetYsize()
{
	return m_ySize;
}

const Voxel::Ordinal & VoxelGrid::GetZsize()
{
	return m_zSize;
}

VoxelGridIterator::VoxelGridIterator(VoxelGrid & grid, bool end) : m_grid(grid)
{
	if (end) {
		m_Xiterator = m_grid.m_voxels.end();
	} else {
		m_Xiterator = m_grid.m_voxels.begin();
		m_Yiterator = (*m_Xiterator).begin();
		m_Ziterator = (*m_Yiterator).begin();
	}
}

std::shared_ptr<Voxel> & VoxelGridIterator::operator*()
{
	return (*m_Ziterator);
}

bool VoxelGridIterator::operator==(const VoxelGridIterator & other)
{
	return m_Xiterator == other.m_Xiterator
		&& (m_Xiterator == m_grid.m_voxels.end()
			|| (m_Yiterator == other.m_Yiterator
				&& (m_Yiterator == (*m_Xiterator).end()
					|| (m_Ziterator == other.m_Ziterator))));
}

bool VoxelGridIterator::operator!=(const VoxelGridIterator & other)
{
	return !(*this == other);
}

VoxelGridIterator & VoxelGridIterator::operator++()
{
	if (m_Xiterator != m_grid.m_voxels.end()) {
		++m_Ziterator;
		if (m_Ziterator == (*m_Yiterator).end()) {
			++m_Yiterator;
			if (m_Yiterator == (*m_Xiterator).end()) {
				++m_Xiterator;
				if (m_Xiterator == m_grid.m_voxels.end())
					return *this;
				m_Yiterator = (*m_Xiterator).begin();
			}
			m_Ziterator = (*m_Yiterator).begin();
		}
	}
	return *this;
}

VoxelGridIterator VoxelGridIterator::operator++(int)
{
	VoxelGridIterator clone(*this);
	++(*this);
	return clone;
}
