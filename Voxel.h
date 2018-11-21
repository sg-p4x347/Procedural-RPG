#pragma once
class VoxelGrid;
struct Voxel
{
public:
	Voxel();
	~Voxel();
	Vector3 GetPosition();
	int GetX();
	int GetY();
	int GetZ();
protected:
	friend class VoxelGrid;
	Voxel(int x, int y, int z);
private:
	int m_x;
	int m_y;
	int m_z;
};

