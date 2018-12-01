#pragma once
#include "ISerialization.h"
class VoxelGrid;
struct Voxel :
	public ISerialization
{
public:
	typedef uint16_t Ordinal;
	Voxel();
	~Voxel();
	Vector3 GetPosition();
	DirectX::BoundingBox Bounds();
	Ordinal GetX();
	Ordinal GetY();
	Ordinal GetZ();
	// Inherited via ISerialization
	virtual void Import(std::istream & ifs) override;
	virtual void Export(std::ostream & ofs) override;
protected:
	friend class VoxelGrid;
	Voxel(Ordinal x, Ordinal y, Ordinal z);
private:
	Voxel::Ordinal m_x;
	Voxel::Ordinal m_y;
	Voxel::Ordinal m_z;

	
};

