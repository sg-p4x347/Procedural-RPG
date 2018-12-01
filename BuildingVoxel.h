#pragma once
#include "Voxel.h"
class BuildingVoxel : 
	public Voxel
{
public:
	typedef uint8_t SubType;

	BuildingVoxel();
	void Wall( int unitX,  int unitZ, SubType type);
	void Floor( int unitX,  int unitZ, SubType floorType);
	void Corner(int unitX, int unitZ, SubType type);
	// Inherited via ISerialization
	virtual void Import(std::istream & ifs) override;
	virtual void Export(std::ostream & ofs) override;
	// Getters
	const SubType* GetFloors();
	const SubType* GetWalls();
	const SubType* GetCorners();

	XMMATRIX TransformWall(unsigned int index);
	// Static Getters
	static const uint8_t GetFloorCount();
	static const uint8_t GetCeilingCount();
	static const uint8_t GetWallCount();
	static const uint8_t GetCornerCount();
private:
	const static uint8_t m_floorCount = 1;
	const static uint8_t m_ceilingCount = 1;
	const static uint8_t m_wallCount = 4;
	const static uint8_t m_cornerCount = 4;
	const static uint8_t m_roofCount = 32;

	SubType m_floors[m_floorCount];
	SubType m_ceilings[m_ceilingCount];
	SubType m_walls[m_wallCount];
	SubType m_corners[m_cornerCount];
	SubType m_roofs[m_roofCount];





	

};

