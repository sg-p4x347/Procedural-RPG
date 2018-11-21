#pragma once
#include "Voxel.h"
class VoxelGrid
{
public:
	friend class VoxelGridIterator;

	VoxelGrid();
	VoxelGrid(size_t xSize, size_t ySize, size_t zSize);
	~VoxelGrid();
	
	// Mutators
	void Set(int x, int y, int z, Voxel && voxel);

	// Accessors
	bool Bounded(int x, int y, int z);
	const size_t GetXsize();
	const size_t GetYsize();
	const size_t GetZsize();
	const std::vector<std::vector<std::shared_ptr<Voxel>>> & operator[](const int index);
	std::set<std::shared_ptr<Voxel>> GetIntersection(DirectX::BoundingFrustum & frustum);
	VoxelGridIterator begin();
	VoxelGridIterator end();
private:
	std::vector<
		std::vector<
			std::vector<std::shared_ptr<Voxel>>
		>
	> m_voxels;
	size_t m_xSize;
	size_t m_ySize;
	size_t m_zSize;
};
class VoxelGridIterator {
public:
	VoxelGridIterator(VoxelGrid & grid, bool end = false);
	typedef std::vector<std::vector<std::vector<std::shared_ptr<Voxel>>>> Xiterator;
	typedef std::vector<std::vector<std::shared_ptr<Voxel>>> Yiterator;
	typedef std::vector<std::shared_ptr<Voxel>> Ziterator;
	std::shared_ptr<Voxel> & operator*();
	bool operator==(const VoxelGridIterator & other);
	bool operator!=(const VoxelGridIterator & other);
	VoxelGridIterator & operator++();
	VoxelGridIterator operator++(int);
	
private:
	VoxelGrid & m_grid;
	std::vector<std::vector<std::vector<std::shared_ptr<Voxel>>>>::iterator m_Xiterator;
	std::vector<std::vector<std::shared_ptr<Voxel>>>::iterator m_Yiterator;
	std::vector<std::shared_ptr<Voxel>>::iterator m_Ziterator;
};
