#pragma once
#include "JSON.h"
class BuildingVoxel : public JSON
{
public:
	BuildingVoxel();
	BuildingVoxel(JsonParser & voxel);
	void Wall( int unitX,  int unitY, unsigned int interiorType);
	void Floor( int unitX,  int unitY, unsigned int floorType);
	// Inherited via JSON
	virtual void Import(JsonParser & jp) override;

	virtual JsonParser Export() override;
private:
	void SetWall(unsigned int cardinalIndex, unsigned int interiorSign, unsigned int interiorType);
	void SetFloor(unsigned int cornerIndex, unsigned int floorType);

	unsigned int m_floors[4];
	unsigned int m_walls[8];
	unsigned int m_corners[4];





};

