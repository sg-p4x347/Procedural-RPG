#pragma once
#include "Mesh.h"
#include "LodGroup.h"
#include "Material.h"
#include "ISerialization.h"
#include <fbxsdk.h>
#include "CollisionModel.h"
#include "CollisionVolume.h"
class AssetEntityManager;
namespace geometry {
	class CMF :
		public ISerialization
	{
	public:
		CMF(string name = "");
		~CMF();
		//----------------------------------------------------------------
		// Accessors
		string GetName() const;
		shared_ptr<CollisionModel> GetCollision();
		void SetCollision(shared_ptr<CollisionModel> collision);
		bool IsAlpha() const;
		shared_ptr<LodGroup> GetLOD(int lod = 0);
		shared_ptr<Material> GetMaterial(string name);
		//----------------------------------------------------------------
		// Modifiers
		int AddLOD(float threshold = 0.f);
		void AddMesh(shared_ptr<Mesh> mesh,int lod = 0);
		void AddMaterial(shared_ptr<Material> material);
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
		// Collision Model import
		void ProcessCollisionNode(fbxsdk::FbxNode * node, shared_ptr<CollisionModel> & collision);
		void ProcessCollisionNodeChildren(fbxsdk::FbxNode * node, shared_ptr<CollisionModel> & collision);
		shared_ptr<CollisionVolume> CreateConvexHull(fbxsdk::FbxMesh * fbxMesh);
		shared_ptr<CollisionVolume> CreateCylinder(fbxsdk::FbxNode * fbxNode);
		//----------------------------------------------------------------
		// Material import
		void ImportMaterials(fbxsdk::FbxScene * scene);
		//----------------------------------------------------------------
		// Texture import
		void ImportTextures(fbxsdk::FbxScene * scene);

		//----------------------------------------------------------------
		// Helpers
		static XMFLOAT3 Convert(fbxsdk::FbxDouble3 & double3);
		static XMFLOAT4 Convert(fbxsdk::FbxDouble4 & double4);
		vector<string> GetTextureConnections(FbxProperty & property);
		static Matrix Convert(fbxsdk::FbxMatrix & matrix);
	private:
		string m_name;
		bool m_alpha;
		vector<shared_ptr<LodGroup>> m_lodGroups;
		map<string,shared_ptr<Material>> m_materials;
		shared_ptr<CollisionModel> m_collision;
		static std::mutex m_mutex;
	};
}
