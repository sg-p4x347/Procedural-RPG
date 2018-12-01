#pragma once
#include "Voxel.h"
#include "ConvexHull.h"
#include "ISerialization.h"
template<typename VoxelType>
class VoxelGrid :
	public ISerialization
{
public:
	friend class VoxelGridIterator;

	inline VoxelGrid() {};
	inline VoxelGrid(Voxel::Ordinal xSize, Voxel::Ordinal ySize, Voxel::Ordinal zSize) :
		m_xSize(xSize),
		m_ySize(ySize),
		m_zSize(zSize),
		m_voxels(xSize, std::vector<std::vector<std::shared_ptr<VoxelType>>>(ySize, std::vector<std::shared_ptr<VoxelType>>(zSize)))
	{
	};
	~VoxelGrid() {};
	
	// Mutators
	void Set(VoxelType && voxel, Voxel::Ordinal x, Voxel::Ordinal y, Voxel::Ordinal z) {
		assert(!Bounded(x, y, z));

		voxel.m_x = x;
		voxel.m_y = y;
		voxel.m_z = z;
		m_voxels[x][y][z] = std::make_shared<Voxel>(std::move(voxel));
	};
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
	const std::vector<std::vector<std::shared_ptr<VoxelType>>> & operator[](const int index) {
		return m_voxels[index];
	};
	std::set<std::shared_ptr<VoxelType>> GetIntersection(DirectX::BoundingFrustum & frustum) {
		std::set<std::shared_ptr<VoxelType>> intersection;
		return intersection;
	};
	std::set<std::shared_ptr<VoxelType>> GetIntersection(geometry::ConvexHull & convexHull) {
		std::set<std::shared_ptr<VoxelType>> broadPhase;
		DirectX::BoundingBox hullBounds = convexHull.Bounds();
		for (auto & voxel : *this) {
			if (voxel && hullBounds.Intersects(voxel->Bounds())) {
				broadPhase.insert(voxel);
			}
		}
		std::set<std::shared_ptr<VoxelType>> narrowPhase;
		for (auto & voxel : broadPhase) {
			CollisionUtil::GjkIntersection intersection;
			auto voxelHull = geometry::ConvexHull(voxel->Bounds());
			if (CollisionUtil::GJK(convexHull.vertices, voxelHull.vertices, intersection)) {
				narrowPhase.insert(voxel);
			}
		}
		return narrowPhase;
	};
	VoxelGridIterator begin() {
		return VoxelGridIterator(*this);
	};
	VoxelGridIterator end() {
		return VoxelGridIterator(*this, true);
	};
	// Inherited via ISerialization
	virtual void Import(std::istream & ifs) {
		DeSerialize(m_xSize, ifs);
		DeSerialize(m_ySize, ifs);
		DeSerialize(m_zSize, ifs);
		for (Voxel::Ordinal x = 0; x < m_xSize; x++) {
			for (Voxel::Ordinal y = 0; y < m_ySize; y++) {
				for (Voxel::Ordinal z = 0; z < m_zSize; z++) {

				}
			}
		}
	};
	virtual void Export(std::ostream & ofs) {
		Serialize(m_xSize, ofs);
		Serialize(m_ySize, ofs);
		Serialize(m_zSize, ofs);
		for (auto & voxel : *this) {
			voxel->Export(ofs);
		}
	};
private:
	std::vector<
		std::vector<
			std::vector<std::shared_ptr<Voxel>>
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
	typedef std::vector<std::vector<std::vector<std::shared_ptr<VoxelType>>>> Xiterator;
	typedef std::vector<std::vector<std::shared_ptr<VoxelType>>> Yiterator;
	typedef std::vector<std::shared_ptr<VoxelType>> Ziterator;
	std::shared_ptr<VoxelType> & operator*() {
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
	std::vector<std::vector<std::vector<std::shared_ptr<VoxelType>>>>::iterator m_Xiterator;
	std::vector<std::vector<std::shared_ptr<VoxelType>>>::iterator m_Yiterator;
	std::vector<std::shared_ptr<VoxelType>>::iterator m_Ziterator;
};
