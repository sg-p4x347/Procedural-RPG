#pragma once
namespace world {
	struct TerrainVertex
	{
	public:
		TerrainVertex();
		~TerrainVertex();
		float terrain;
		float water;
		Vector3 terrainNormal;
		Vector3 waterNormal;
	};
}
