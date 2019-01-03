#include "pch.h"
#include "ModelVoxel.h"

ModelVoxel::ModelVoxel() {}

void ModelVoxel::AddComponent(AssetID asset, TransformID transform)
{
	m_components.push_back(std::pair<AssetID, TransformID>(asset, transform));
}

void ModelVoxel::AddComponent(AssetID asset, Transforms transform)
{
	AddComponent(asset, (TransformID)transform);
}

const std::vector<std::pair<AssetID, TransformID>>& ModelVoxel::GetComponents() const
{
	return m_components;
}

shared_ptr<geometry::CollisionModel> ModelVoxel::GetCollision()
{
	return m_collision;
}

void ModelVoxel::SetCollision(shared_ptr<geometry::CollisionModel>& collision)
{
	m_collision = collision;
}

void ModelVoxel::Import(std::istream & ifs)
{
	size_t size;
	DeSerialize(size, ifs);
	for (int i = 0; i < size; i++) {
		std::pair<AssetID, TransformID> component;
		DeSerialize(component.first, ifs);
		DeSerialize(component.second, ifs);
		m_components.push_back(component);
	}
}

void ModelVoxel::Export(std::ostream & ofs)
{
	Serialize(m_components.size(),ofs);
	for (auto & modelComponent : m_components) {
		Serialize(modelComponent.first, ofs);
		Serialize(modelComponent.second, ofs);
	}
}