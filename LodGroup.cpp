#include "pch.h"
#include "LodGroup.h"
#include "CustomModelLoadVBO.h"
namespace geometry {
	LodGroup::LodGroup(float threshold) : m_threshold(threshold)
	{
	}

	LodGroup::LodGroup(std::vector<shared_ptr<Mesh>>& meshes, float threshold) : m_threshold(threshold), m_meshes(meshes)
	{
	}

	const std::vector<shared_ptr<Mesh>>& geometry::LodGroup::GetMeshes() const
	{
		return m_meshes;
	}

	const float LodGroup::GetThreshold() const
	{
		return m_threshold;
	}

	std::shared_ptr<DirectX::Model> LodGroup::GetModel(ID3D11Device * d3dDevice, IEffectFactory * fxFactory)
	{
		if (!m_model) {
			m_model = std::make_shared<DirectX::Model>();
			for (auto & mesh : m_meshes) {
				shared_ptr<DirectX::ModelMesh> dxMesh = std::make_shared<DirectX::ModelMesh>();
				for (auto & part : mesh->GetParts()) {
					auto effect = part.material->GetEffect(fxFactory);
					auto temp = CustomModelLoadVBO::CreateFromVBO(d3dDevice, part.vertices, part.indices, effect, true);
					auto && dxPart = temp->meshes[0]->meshParts[0];
					dxPart->isAlpha = part.alpha;
					dxMesh->meshParts.push_back(std::make_unique<DirectX::ModelMeshPart>(*(dxPart)));
				}
				m_model->meshes.push_back(dxMesh);
			}
		}
		return m_model;
	}

	void LodGroup::ModelChanged()
	{
		m_model = nullptr;
	}

	void LodGroup::AddMesh(shared_ptr<Mesh> mesh)
	{
		m_meshes.push_back(mesh);
	}


	LodGroup::~LodGroup()
	{
	}
	void LodGroup::Import(std::istream & ifs)
	{
		DeSerialize(m_threshold, ifs);
		size_t meshCount = 0;
		DeSerialize(meshCount, ifs);
		for (int meshIndex = 0; meshIndex < meshCount; meshIndex++) {
			auto mesh = std::make_shared<Mesh>();
			mesh->Import(ifs);
			m_meshes.push_back(mesh);
		}
	}
	void LodGroup::Export(std::ostream & ofs)
	{
		Serialize(m_threshold, ofs);
		Serialize(m_meshes.size(), ofs);
		for (auto & mesh : m_meshes)
			mesh->Export(ofs);
	}
}