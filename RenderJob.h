#pragma once
#include "AssetTypedefs.h"
#include "EntityTypedefs.h"

struct RenderJob {
	RenderJob(
		world::EntityID entity,
		Vector3 position,
		Vector3 rotation,
		AssetID asset,
		AssetType assetType = AssetType::Authored
	);
	world::EntityID entity;
	AssetID modelAsset;
	AssetType assetType;
	Vector3 position;
	Matrix worldMatrix;
};

