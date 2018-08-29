#pragma once
#include "ISerialization.h"
class BuildingVoxel : public ISerialization
{
public:
	BuildingVoxel();
	void Wall( int unitX,  int unitZ, unsigned int type);
	void Floor( int unitX,  int unitZ, unsigned int floorType);
	void Corner(int unitX, int unitZ, unsigned int type);
	// Inherited via ISerialization
	virtual void Import(std::istream & ifs) override;
	virtual void Export(std::ostream & ofs) override;
	// Getters
	const UINT* GetFloors();
	const UINT* GetWalls();
	const UINT* GetCorners();

	XMMATRIX TransformWall(unsigned int index);
	// Static Getters
	static const UINT GetFloorCount();
	static const UINT GetWallCount();
	static const UINT GetCornerCount();
private:
	const static UINT m_floorCount = 1;
	const static UINT m_wallCount = 4;
	const static UINT m_cornerCount = 4;

	unsigned int m_floors[m_floorCount];
	unsigned int m_walls[m_wallCount];
	unsigned int m_corners[m_cornerCount];






	

};

