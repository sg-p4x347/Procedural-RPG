#include "pch.h"
#include "BuildingVoxel.h"

using namespace DirectX;
using namespace Utility;
BuildingVoxel::BuildingVoxel()
{
	InitArray<BuildingVoxel::SubType>(m_floors, m_floorCount, 0);
	InitArray<BuildingVoxel::SubType>(m_ceilings, m_ceilingCount, 0);
	InitArray<BuildingVoxel::SubType>(m_walls, m_wallCount, 0);
	InitArray<BuildingVoxel::SubType>(m_corners, m_cornerCount, 0);
}

void BuildingVoxel::Wall(int unitX, int unitZ, BuildingVoxel::SubType type)
{
	/*
	^
	|
	+X
			  1
	 +--------+--------+
	 |		  |		   |
	 |		  |		   |
	 |		  |		   |
	2+--------*--------+0
	 |		  |		   |
	 |		  |		   |
	 |		  |		   |
	 +--------+--------+
		 	  3
	+Z --->

	The '*' in the above diagram represents the local
	origin of this voxel

	Each '+' in the above diagram represents a valid
	Unit square position; the two interior walls that
	intersect this point are selected
	*/
	if (abs(unitZ) == 1)
		m_walls[2 - (unitZ + 1)] = type;

	if (abs(unitX) == 1) {
		m_walls[3 - (unitX + 1)] = type;
	}
}

void BuildingVoxel::Floor(int unitX,  int unitY, BuildingVoxel::SubType floorType)
{
	m_floors[0] = floorType;
}

void BuildingVoxel::Corner(int unitX, int unitZ, BuildingVoxel::SubType type)
{
	/*
	^
	|
	+X
	1				  0
	+-----------------+
	|		 |		  |
	|		 |		  |
	|		 |		  |
	|--------*--------|
	|		 |		  |
	|		 |		  |
	|		 |		  |
	+-----------------+
	2				  3
	+Z --->
	*/
	if (unitZ == 1) {
		if (unitX == 1) {
			m_corners[0] = type;
		}
		else {
			m_corners[3] = type;
		}
	}
	else {
		if (unitX == 1) {
			m_corners[1] = type;
		}
		else {
			m_corners[2] = type;
		}
	}
}

void BuildingVoxel::Import(std::istream & ifs)
{
	for (int i = 0; i < m_floorCount; i++) {
		DeSerialize(m_floors[i],ifs);
	}
	for (int i = 0; i < m_wallCount; i++) {
		DeSerialize(m_walls[i], ifs);
	}
	for (int i = 0; i < m_cornerCount; i++) {
		DeSerialize(m_corners[i], ifs);
	}
}

void BuildingVoxel::Export(std::ostream & ofs)
{
	for (int i = 0; i < m_floorCount; i++) {
		Serialize(m_floors[i], ofs);
	}
	for (int i = 0; i < m_wallCount; i++) {
		Serialize(m_walls[i], ofs);
	}
	for (int i = 0; i < m_cornerCount; i++) {
		Serialize(m_corners[i], ofs);
	}
}

const BuildingVoxel::SubType * BuildingVoxel::GetFloors()
{
	return m_floors;
}

const BuildingVoxel::SubType * BuildingVoxel::GetWalls()
{
	return m_walls;
}

const BuildingVoxel::SubType * BuildingVoxel::GetCorners()
{
	return m_corners;
}

XMMATRIX BuildingVoxel::TransformWall(unsigned int index)
{

	return XMMATRIX();
}

const uint8_t BuildingVoxel::GetFloorCount()
{
	return m_floorCount;
}

const uint8_t BuildingVoxel::GetCeilingCount()
{
	return m_ceilingCount;
}

const uint8_t BuildingVoxel::GetWallCount()
{
	return m_wallCount;
}

const uint8_t BuildingVoxel::GetCornerCount()
{
	return m_cornerCount;
}
