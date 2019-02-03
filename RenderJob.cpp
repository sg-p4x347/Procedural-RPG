#include "pch.h"
#include "RenderJob.h"

RenderJob::RenderJob(
	world::EntityID entity, 
	Vector3 position, 
	Vector3 rotation, 
	AssetID asset,
	AssetType type) :
	entity(entity),
	position(position),
	worldMatrix(Matrix::CreateFromYawPitchRoll(rotation.y,rotation.x,rotation.z) * Matrix::CreateTranslation(position)),
	modelAsset(asset),
	assetType(type)
{
}
