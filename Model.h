#pragma once
#include "WorldComponent.h"
#include "AssetTypes.h"
namespace world {
	
	class Model :
		public WorldComponent
	{
	public:
		Model();
		Model(EntityID asset,AssetType type);
		
		EntityID Asset;
		AssetType Type;
	};
}