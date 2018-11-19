#include "pch.h"
#include "LodGroup.h"
#include "CustomModelLoadVBO.h"
namespace geometry {
	LodGroup::LodGroup() : m_threshold(0.f)
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
			MeshPart & part = m_meshes[0]->GetParts()[0];
			auto effect = part.material->GetEffect(fxFactory);
			m_model = CustomModelLoadVBO::CreateFromVBO(d3dDevice, part.vertices, part.indices, effect,true);
		}
		return m_model;
	}

	void LodGroup::AddMesh(shared_ptr<Mesh> mesh)
	{
		m_meshes.push_back(mesh);
	}


	LodGroup::~LodGroup()
	{
	}
}