#pragma once
#include "JSON.h"
class Voxel : public JSON
{
public:
	Voxel();
	Voxel(JsonParser & voxel);
	void Wall(short unitX, short unitY, short interiorType);
	void Floor(short unitX, short unitY, short floorType);
	// Inherited via JSON
	virtual void Import(JsonParser & jp) override;

	virtual JsonParser Export() override;
private:
	void SetWall(short cardinalIndex, short interiorSign, short interiorType);
	void SetFloor(short cornerIndex, short floorType);
	
	short m_floors[4];
	short m_walls[8];
	short m_corners[4];
	


	

};

