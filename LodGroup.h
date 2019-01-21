#pragma once
#include "Mesh.h"
#include "ISerialization.h"
namespace geometry {
	class LodGroup :
		public ISerialization
	{
	public:
		LodGroup(float threshold = 0.f);
		LodGroup(std::vector<shared_ptr<Mesh>> & meshes, float threshold = 0.f);
		const std::vector<shared_ptr<Mesh>> & GetMeshes() const;
		const float GetThreshold() const;
		std::shared_ptr<DirectX::Model> GetModel(ID3D11Device * d3dDevice, IEffectFactory * fxFactory);
		void ModelChanged();
		void AddMesh(shared_ptr<Mesh> mesh);
		~LodGroup();
		// Inherited via ISerialization
		virtual void Import(std::istream & ifs) override;
		virtual void Export(std::ostream & ofs) override;
	private:
		float m_threshold;
		std::vector<shared_ptr<Mesh>> m_meshes;
		shared_ptr<DirectX::Model> m_model;

		
	};
}

