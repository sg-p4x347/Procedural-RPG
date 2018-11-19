#pragma once
#include "Mesh.h"

namespace geometry {
	class LodGroup
	{
	public:
		LodGroup();
		LodGroup(std::vector<shared_ptr<Mesh>> & meshes, float threshold = 0.f);
		const std::vector<shared_ptr<Mesh>> & GetMeshes() const;
		const float GetThreshold() const;
		std::shared_ptr<DirectX::Model> GetModel(ID3D11Device * d3dDevice, IEffectFactory * fxFactory);
		void AddMesh(shared_ptr<Mesh> mesh);
		~LodGroup();
	private:
		float m_threshold;
		std::vector<shared_ptr<Mesh>> m_meshes;
		shared_ptr<DirectX::Model> m_model;
	};
}

