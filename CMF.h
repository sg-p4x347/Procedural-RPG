#pragma once
#include "Mesh.h"
#include "LodGroup.h"
#include "Material.h"
#include "ISerialization.h"
#include <fbxsdk.h>
class AssetEntityManager;
namespace geometry {
	class CMF :
		public ISerialization
	{
	public:
		CMF(string name);
		~CMF();
		//----------------------------------------------------------------
		// Accessors
		string GetName() const;
		//----------------------------------------------------------------
		// Modifiers
		void AddMesh(shared_ptr<Mesh> mesh);
		void SetName(string name);
		//----------------------------------------------------------------
		// IO
		void Export(Filesystem::path directory);
		shared_ptr<DirectX::Model> GetModel(ID3D11Device * d3dDevice,IEffectFactory * fxFactory,float distance = 0.f);
		//----------------------------------------------------------------
		// Inherited via ISerialization
		virtual void Import(std::istream & ifs) override;
		virtual void Export(std::ostream & ofs) override;
		//----------------------------------------------------------------
		// FBX import; documentation: http://help.autodesk.com/view/FBX/2019/ENU/?guid=FBX_Developer_Help_getting_started_html
		static shared_ptr<CMF> CreateFromFBX(Filesystem::path path, AssetEntityManager * entityManager);
	private:
		//----------------------------------------------------------------
		// FBX import
		void Import(fbxsdk::FbxScene * scene);
		//----------------------------------------------------------------
		// Mesh import
		void ProcessNode(fbxsdk::FbxNode * node, std::vector<shared_ptr<Mesh>> & meshes);
		void ProcessNodeChildren(fbxsdk::FbxNode * node, std::vector<shared_ptr<Mesh>> & meshes);
		shared_ptr<Mesh> CreateMesh(fbxsdk::FbxMesh * fbxMesh);
		//----------------------------------------------------------------
		// Material import
		void ImportMaterials(fbxsdk::FbxScene * scene);
		//----------------------------------------------------------------
		// Texture import
		void ImportTextures(fbxsdk::FbxScene * scene);

		//----------------------------------------------------------------
		// Helpers
		XMFLOAT3 Convert(fbxsdk::FbxDouble3 & double3);
		vector<string> GetTextureConnections(FbxProperty & property);
	private:
		string m_name;
		vector<shared_ptr<Mesh>> m_meshes;
		vector<LodGroup> m_lodGroups;
		map<string,shared_ptr<Material>> m_materials;
	};
}
