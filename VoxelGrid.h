#pragma once
#include "Voxel.h"
#include "ConvexHull.h"
#include "ISerialization.h"

template<typename VoxelType>
class VoxelGridIterator;

template<typename VoxelType>
class VoxelGrid :
	public ISerialization
{
public:
	friend class VoxelGridIterator<VoxelType>;

	inline VoxelGrid() : VoxelGrid<VoxelType>::VoxelGrid(0,0,0) {};
	inline VoxelGrid(Voxel::Ordinal xSize, Voxel::Ordinal ySize, Voxel::Ordinal zSize) :
		m_xSize(xSize),
		m_ySize(ySize),
		m_zSize(zSize),
		m_voxels(xSize, std::vector<std::vector<VoxelType>>(ySize, std::vector<VoxelType>(zSize)))
	{
		// initialize voxel positions
		for (Voxel::Ordinal x = 0; x < xSize; x++) {
			for (Voxel::Ordinal y = 0; y < ySize; y++) {
				for (Voxel::Ordinal z = 0; z < zSize; z++) {
					m_voxels[x][y][z].SetPosition(x, y, z);
				}
			}
		}
	};
	~VoxelGrid() {};
	
	// Mutators
	void Set(VoxelType & voxel, Voxel::Ordinal x, Voxel::Ordinal y, Voxel::Ordinal z) {
		assert(Bounded(x, y, z));

		voxel.SetPosition(x, y, z);
		m_voxels[x][y][z] = voxel;
	};
	void Resize(Voxel::Ordinal xSize, Voxel::Ordinal ySize, Voxel::Ordinal zSize) {
		if (m_xSize < xSize) {
			// expand existing vectors
			for (Voxel::Ordinal x = 0; x < m_xSize; x++) {
				m_voxels[x].resize(ySize, std::vector<VoxelType>(zSize));
			}
			// add new vectors
			m_voxels.resize(xSize,std::vector<std::vector<VoxelType>>(ySize, std::vector<VoxelType>(zSize)));
			m_xSize = xSize;
			m_ySize = ySize;
			m_zSize = zSize;
		}
	}
	// Accessors
	bool Bounded(Voxel::Ordinal x, Voxel::Ordinal y, Voxel::Ordinal z) {
		return
			x < m_xSize
			&&
			y < m_ySize
			&&
			z < m_zSize;

	};
	const Voxel::Ordinal & GetXsize() {
		return m_xSize;
	};
	const Voxel::Ordinal & GetYsize() {
		return m_ySize;
	};
	const Voxel::Ordinal & GetZsize() {
		return m_zSize;
	};
	std::vector<std::vector<VoxelType>> & operator[](const int index) {
		return m_voxels[index];
	};
	std::vector<VoxelType> GetIntersection(DirectX::BoundingFrustum & frustum) {
		std::vector<VoxelType> intersection;
		for (auto & voxel : *this) {
			if (frustum.Intersects(voxel.Bounds())) {
				intersection.push_back(voxel);
			}
		}
		return intersection;
	};
	std::vector<VoxelType> GetIntersection(geometry::ConvexHull & convexHull) {
		std::vector<VoxelType> results;
		for (auto & voxel : GetIntersection(convexHull.Bounds())) {
			CollisionUtil::GjkIntersection intersection;
			auto voxelHull = geometry::ConvexHull(voxel.Bounds());
			if (CollisionUtil::GJK(convexHull.vertices, voxelHull.vertices, intersection)) {
				results.push_back(voxel);
			}
		}
		return results;
	};
	std::vector<VoxelType> GetIntersection(BoundingBox & box) {
		std::vector<VoxelType> intersection;
		for (auto & voxel : *this) {
			if (box.Intersects(voxel.Bounds())) {
				intersection.push_back(voxel);
			}
		}
		return intersection;
	}
	VoxelGridIterator<VoxelType> begin() {
		return VoxelGridIterator(*this);
	};
	VoxelGridIterator<VoxelType>  end() {
		return VoxelGridIterator(*this, true);
	};
	// Inherited via ISerialization
	virtual void Import(std::istream & ifs) {
		Voxel::Ordinal xSize;
		Voxel::Ordinal ySize;
		Voxel::Ordinal zSize;
		DeSerialize(xSize, ifs);
		DeSerialize(ySize, ifs);
		DeSerialize(zSize, ifs);
		Resize(xSize, ySize, zSize);
		for (Voxel::Ordinal x = 0; x < m_xSize; x++) {
			for (Voxel::Ordinal y = 0; y < m_ySize; y++) {
				for (Voxel::Ordinal z = 0; z < m_zSize; z++) {
					VoxelType voxel; // default construct
					voxel.Import(ifs);
					Set(voxel, x, y, z);
				}
			}
		}
	};
	virtual void Export(std::ostream & ofs) {
		Serialize(m_xSize, ofs);
		Serialize(m_ySize, ofs);
		Serialize(m_zSize, ofs);
		for (Voxel::Ordinal x = 0; x < m_xSize; x++) {
			for (Voxel::Ordinal y = 0; y < m_ySize; y++) {
				for (Voxel::Ordinal z = 0; z < m_zSize; z++) {
					m_voxels[x][y][z].Export(ofs);
				}
			}
		}
	};
private:
	std::vector<
		std::vector<
			std::vector<VoxelType>
		>
	> m_voxels;
	Voxel::Ordinal m_xSize;
	Voxel::Ordinal m_ySize;
	Voxel::Ordinal m_zSize;
};
template<typename VoxelType>
class VoxelGridIterator {
public:
	VoxelGridIterator(VoxelGrid<VoxelType> & grid, bool end = false) : m_grid(grid)
	{
		if (end) {
			m_Xiterator = m_grid.m_voxels.end();
		}
		else {
			m_Xiterator = m_grid.m_voxels.begin();
			m_Yiterator = (*m_Xiterator).begin();
			m_Ziterator = (*m_Yiterator).begin();
		}
	};
	typedef std::vector<std::vector<std::vector<VoxelType>>> Xiterator;
	typedef std::vector<std::vector<VoxelType>> Yiterator;
	typedef std::vector<VoxelType> Ziterator;
	VoxelType & operator*() {
		return (*m_Ziterator);
	};
	bool operator==(const VoxelGridIterator & other) {
		return m_Xiterator == other.m_Xiterator
			&& (m_Xiterator == m_grid.m_voxels.end()
				|| (m_Yiterator == other.m_Yiterator
					&& (m_Yiterator == (*m_Xiterator).end()
						|| (m_Ziterator == other.m_Ziterator))));
	};
	bool operator!=(const VoxelGridIterator & other) {
		return !(*this == other);
	};
	VoxelGridIterator & operator++() {
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
	};
	VoxelGridIterator operator++(int) {
		VoxelGridIterator clone(*this);
		++(*this);
		return clone;
	};
	
private:
	VoxelGrid<VoxelType> & m_grid;
	typename std::vector<std::vector<std::vector<VoxelType>>>::iterator m_Xiterator;
	typename std::vector<std::vector<VoxelType>>::iterator m_Yiterator;
	typename std::vector<VoxelType>::iterator m_Ziterator;
};
