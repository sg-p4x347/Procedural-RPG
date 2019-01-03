#pragma once
#include "ISerialization.h"
struct Voxel :
	public ISerialization
{
public:
	typedef uint16_t Ordinal;
	Voxel();
	Voxel(Ordinal x, Ordinal y, Ordinal z);
	~Voxel();
	Vector3 GetPosition();
	void SetPosition(Ordinal x, Ordinal y, Ordinal z);
	DirectX::BoundingBox Bounds();
	Ordinal GetX();
	Ordinal GetY();
	Ordinal GetZ();
	// Inherited via ISerialization
	virtual void Import(std::istream & ifs) override;

	virtual void Export(std::ostream & ofs) override;
private:
	Voxel::Ordinal m_x;
	Voxel::Ordinal m_y;
	Voxel::Ordinal m_z;
};

