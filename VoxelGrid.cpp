#include "pch.h"
#include "VoxelGrid.h"


VoxelGrid::VoxelGrid()
{
}

VoxelGrid::VoxelGrid(int xSize, int ySize, int zSize) :
	m_voxels(xSize,std::vector<std::vector<std::shared_ptr<Voxel>>>(ySize,std::vector<std::shared_ptr<Voxel>>(zSize)))
{
}


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

VoxelGridIterator VoxelGrid::begin()
{
	return VoxelGridIterator(*this);
}

VoxelGridIterator VoxelGrid::end()
{
	return VoxelGridIterator(*this,true);
}

void VoxelGrid::Set(int x, int y, int z, Voxel && voxel)
{
	assert(!Bounded(x, y, z));
	m_voxels[x][y][z];
}

bool VoxelGrid::Bounded(int x, int y, int z)
{
	return
		x >= 0 && x < GetXsize()
		&&
		y >= 0 && y < GetYsize()
		&&
		z >= 0 && z < GetZsize();

}

const size_t VoxelGrid::GetXsize()
{
	return m_voxels.size();
}

const size_t VoxelGrid::GetYsize()
{
	return m_voxels[0].size();
}

const size_t VoxelGrid::GetZsize()
{
	return m_voxels[0][0].size();
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
	return &m_grid == &(other.m_grid)
		&& m_Xiterator == other.m_Xiterator
		&& m_Yiterator == other.m_Yiterator
		&& m_Ziterator == other.m_Ziterator;
}

bool VoxelGridIterator::operator!=(const VoxelGridIterator & other)
{
	return !(*this == other);
}

VoxelGridIterator & VoxelGridIterator::operator++()
{
	++m_Ziterator;
	if (m_Ziterator == (*m_Yiterator).end()) {
		++m_Yiterator;
		if (m_Yiterator == (*m_Xiterator).end()) {
			++m_Xiterator;
			m_Yiterator = (*m_Xiterator).begin();
		}
		m_Ziterator = (*m_Yiterator).begin();
	}
	return *this;
}

VoxelGridIterator VoxelGridIterator::operator++(int)
{
	VoxelGridIterator clone(*this);
	++(*this);
	return clone;
}
