#include "pch.h"
#include "VoxelGrid.h"


VoxelGrid::VoxelGrid()
{
}

VoxelGrid::VoxelGrid(size_t xSize, size_t ySize, size_t zSize) :
	m_xSize(xSize),
	m_ySize(ySize),
	m_zSize(zSize),
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
		x >= 0 && x < m_xSize
		&&
		y >= 0 && y < m_ySize
		&&
		z >= 0 && z < m_zSize;

}

const size_t VoxelGrid::GetXsize()
{
	return m_xSize;
}

const size_t VoxelGrid::GetYsize()
{
	return m_ySize;
}

const size_t VoxelGrid::GetZsize()
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
	return m_Xiterator == other.m_Xiterator;
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
				if (m_Xiterator == m_grid.m_voxels.end()) {
					return *this;
				}
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
