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
		
		// Relative path for locating the model
		EntityID Asset;
		// Determines how to import this model
		AssetType Type;

		//// Effect used to render the model
		//string Effect;
		//// Cull back faces
		//bool BackfaceCulling;

		// Inherited via Component
	};
}