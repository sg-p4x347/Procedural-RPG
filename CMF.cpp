#include "pch.h"
#include "CMF.h"
#include <sstream>

#include "AssetEntityManager.h"
#include "Filesystem.h"

namespace geometry {


	CMF::CMF(string name) : m_name(name)
	{
	}

	CMF::~CMF()
	{
	}
	string CMF::GetName() const
	{
		return m_name;
	}
	const CollisionModel CMF::GetCollision() const
	{
		return m_collision;
	}
	void CMF::AddMesh(shared_ptr<Mesh> mesh)
	{
		m_meshes.push_back(mesh);
	}
	void CMF::SetName(string name)
	{
		m_name = name;
	}
	/**
	* Return a string-based representation based on the attribute type.
	*/
	FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) {
		switch (type) {
		case FbxNodeAttribute::eUnknown: return "unidentified";
		case FbxNodeAttribute::eNull: return "null";
		case FbxNodeAttribute::eMarker: return "marker";
		case FbxNodeAttribute::eSkeleton: return "skeleton";
		case FbxNodeAttribute::eMesh: return "mesh";
		case FbxNodeAttribute::eNurbs: return "nurbs";
		case FbxNodeAttribute::ePatch: return "patch";
		case FbxNodeAttribute::eCamera: return "camera";
		case FbxNodeAttribute::eCameraStereo: return "stereo";
		case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
		case FbxNodeAttribute::eLight: return "light";
		case FbxNodeAttribute::eOpticalReference: return "optical reference";
		case FbxNodeAttribute::eOpticalMarker: return "marker";
		case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
		case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
		case FbxNodeAttribute::eBoundary: return "boundary";
		case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
		case FbxNodeAttribute::eShape: return "shape";
		case FbxNodeAttribute::eLODGroup: return "lodgroup";
		case FbxNodeAttribute::eSubDiv: return "subdiv";
		default: return "unknown";
		}
	}
	void PrintTabs(int & numTabs, ostream & out) {
		for (int i = 0; i < numTabs; i++)
			out << '\t';
	}
	void DisplayProperties(int & numTabs, ostream & out,fbxsdk::FbxObject * object) {
		PrintTabs(numTabs, out);
		out << "Properties" << std::endl << std::endl;
		numTabs++;
		auto property = object->GetFirstProperty();
		while (property.IsValid()) {
			PrintTabs(numTabs, out);
			out << "(" << property.GetPropertyDataType().GetName() << ") ";
			out << property.GetHierarchicalName().Buffer() << " : " << property.Get<fbxsdk::FbxString>().Buffer();
			out << std::endl;
			property = object->GetNextProperty(property);
		}
		numTabs--;
	}
	/**
	 * Print an attribute.
	 */
	void PrintAttribute(int & numTabs, ostream & out, FbxNodeAttribute* pAttribute) {
		if (!pAttribute) return;

		FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
		FbxString attrName = pAttribute->GetName();
		
		// Note: to retrieve the character array of a FbxString, use its Buffer() method.
		PrintTabs(numTabs, out);
		out << typeName.Buffer() << " : " << attrName.Buffer() << std::endl;
		// Print the attribute's properties
		DisplayProperties(numTabs,out,pAttribute);
		
	}
	/**
	 * Print a node, its attributes, and all its children recursively.
	 */
	void PrintNode(int & numTabs, ostream & out, FbxNode* pNode) {
		const char* nodeName = pNode->GetName();
		FbxDouble3 translation = pNode->LclTranslation.Get();
		FbxDouble3 rotation = pNode->LclRotation.Get();
		FbxDouble3 scaling = pNode->LclScaling.Get();

		// Print the contents of the node.
		PrintTabs(numTabs, out);
		out << "__________Node__________" << std::endl;
		PrintTabs(numTabs, out);
		out << "Name : " << nodeName << std::endl;
		PrintTabs(numTabs, out);
		out << "Translation : (" << translation[0] << ',' << translation[1] << ',' << translation[2] << ')' << std::endl;
		PrintTabs(numTabs, out);
		out << "Rotation : (" << rotation[0] << ',' << rotation[1] << ',' << rotation[2] << ')' << std::endl;
		PrintTabs(numTabs, out);
		out	<< "Scaling : (" << scaling[0] << ',' << scaling[1] << ',' << scaling[2] << ')' << std::endl;
		numTabs++;

		// Print the node's attributes.
		PrintTabs(numTabs, out);
		out << "Attributes" << std::endl << std::endl;
		numTabs++;
		for (int i = 0; i < pNode->GetNodeAttributeCount(); i++)
			PrintAttribute(numTabs,out,pNode->GetNodeAttributeByIndex(i));

		numTabs--;
		out << std::endl;
		
		// Recursively print the children.
		PrintTabs(numTabs, out);
		out << "Children" << std::endl << std::endl;
		numTabs++;
		for (int j = 0; j < pNode->GetChildCount(); j++)
			PrintNode(numTabs, out, pNode->GetChild(j));
		numTabs--;
		out << std::endl;
		numTabs--;
		
	}
	shared_ptr<CMF> CMF::CreateFromFBX(Filesystem::path path, AssetEntityManager * entityManager)
	{
		string pathString = path.string();
		// Change the following filename to a suitable filename value.
		const char* lFilename = pathString.c_str();

		// Initialize the SDK manager. This object handles memory management.
		fbxsdk::FbxManager* lSdkManager = fbxsdk::FbxManager::Create();

		// Create the IO settings object.
		fbxsdk::FbxIOSettings *ios = fbxsdk::FbxIOSettings::Create(lSdkManager, IOSROOT);
		lSdkManager->SetIOSettings(ios);

		// Create an importer using the SDK manager.
		fbxsdk::FbxImporter* lImporter = fbxsdk::FbxImporter::Create(lSdkManager, "");

		// Use the first argument as the filename for the importer.
		if (!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings())) {
			throw std::exception(("Call to FbxImporter::Initialize() failed. Error returned: " + string(lImporter->GetStatus().GetErrorString())).c_str());
		}
		// Create a new scene so that it can be populated by the imported file.
		fbxsdk::FbxScene* lScene = fbxsdk::FbxScene::Create(lSdkManager, "myScene");

		// Import the contents of the file into the scene.
		lImporter->Import(lScene);

		// The file is imported, so get rid of the importer.
		lImporter->Destroy();

		// Print the nodes of the scene and their attributes recursively.
		// Note that we are not printing the root node because it should
		// not contain any attributes.
		FbxNode* lRootNode = lScene->GetRootNode();
		if (lRootNode) {
			int numTabs = 0;
			std::ostringstream ss;
			
			for (int i = 0; i < lRootNode->GetChildCount(); i++) {
				auto child = lRootNode->GetChild(i);
				PrintNode(numTabs, ss, child);
			}  

			string str = ss.str();
			OutputDebugStringA(str.c_str());
		}
		// generate a CMF instance
		auto cmf = std::make_shared<CMF>(FileSystemHelpers::FilenameWithoutExtension(path));
		
		cmf->Import(lScene);
		
		// Destroy the SDK manager and all the other objects it was handling.
		lSdkManager->Destroy();

		// check if there is not an asset entity created with this name
		EntityPtr entity;
		if (!entityManager->TryFindByPathID(cmf->GetName(), entity)) {
			entity = entityManager->NewEntity();
			entity->AddComponent(new ModelAsset());
			entity->AddComponent(new PathID(cmf->GetName()));
		}

		return cmf;
	}
	void CMF::ProcessNode(fbxsdk::FbxNode * node, std::vector<shared_ptr<Mesh>> & meshes)
	{
		auto attribute = node->GetNodeAttribute();
		if (attribute) {
			auto type = attribute->GetAttributeType();
			// Attribute type determines how to process child nodes
			if (type == fbxsdk::FbxNodeAttribute::eLODGroup) {
				for (int i = 0; i < node->GetChildCount(); i++) {
					std::vector<shared_ptr<Mesh>> lodMeshes;
					ProcessNodeChildren(node->GetChild(i), lodMeshes);
					// create a lod group for these meshes
					auto threshold = attribute->FindPropertyHierarchical(("Thresholds|Level" + to_string(i)).c_str());
					auto lodGroup = LodGroup(lodMeshes, threshold.IsValid() ? threshold.Get<FbxDistance>().value() : std::numeric_limits<float>::infinity());
					m_lodGroups.push_back(lodGroup);
					// add these meshes to parent meshes vector
					meshes.insert(meshes.end(), lodMeshes.begin(), lodMeshes.end());
				}
			}
			else if (type == fbxsdk::FbxNodeAttribute::eMesh) {
				meshes.push_back(CreateMesh((FbxMesh *)attribute));
			}
			else {
				ProcessNodeChildren(node, meshes);
			}
		}
		else {
			ProcessNodeChildren(node, meshes);
		}
	}
	void CMF::ProcessNodeChildren(fbxsdk::FbxNode * node, std::vector<shared_ptr<Mesh>>& meshes)
	{
		for (int i = 0; i < node->GetChildCount(); i++) {
			auto child = node->GetChild(i);
			string name = child->GetNameOnly();
			if (name == "collision") {
				ProcessCollisionNode(child, m_collision);
			}
			else {
				ProcessNode(child, meshes);
			}
		}
	}
	shared_ptr<Mesh> CMF::CreateMesh(fbxsdk::FbxMesh * fbxMesh)
	{
		shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		MeshPart part;
		int index = 0;
		for (int polyIndex = 0; polyIndex < fbxMesh->GetPolygonCount();polyIndex++) {
			
			int vertexCount = fbxMesh->GetPolygonSize(polyIndex);
			int polygonIndexStart = fbxMesh->GetPolygonVertexIndex(polyIndex);
			int controlPointIndex = polygonIndexStart + polyIndex;
			for (int polyVertIndex = 0; polyVertIndex < vertexCount; polyVertIndex++) {
				VertexPositionNormalTangentColorTexture vertex;
				auto transform = fbxMesh->GetNode()->EvaluateGlobalTransform();
				auto control = transform.MultT(fbxMesh->GetControlPointAt(fbxMesh->GetPolygonVertex(polyIndex, polyVertIndex)).mData);
				//auto control = fbxMesh->GetControlPointAt(fbxMesh->GetPolygonVertex(polyIndex, polyVertIndex)).mData;
				vertex.position.x = control[0];
				vertex.position.y = control[1];
				vertex.position.z = control[2];
				fbxsdk::FbxVector4 normal;
				if (fbxMesh->GetPolygonVertexNormal(polyIndex, polyVertIndex, normal)) {
					normal = transform.MultT(normal) - transform.MultT(fbxsdk::FbxZeroVector4);
					Vector3 normalVector = Vector3(normal[0], normal[1], normal[2]);
					normalVector.Normalize();
					vertex.normal = normalVector;
					
				}
				for (int uvIndex = 0; uvIndex < fbxMesh->GetElementUVCount(); uvIndex++) {
					auto uvElement = fbxMesh->GetElementUV(uvIndex);
					switch (uvElement->GetMappingMode())
					{
					case FbxGeometryElement::eByControlPoint:
						break;
					case FbxGeometryElement::eByPolygonVertex:
					{
						int lTextureUVIndex = fbxMesh->GetTextureUVIndex(polyIndex,polyVertIndex);
						switch (uvElement->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
						case FbxGeometryElement::eIndexToDirect:
						{
							auto uv = uvElement->GetDirectArray().GetAt(lTextureUVIndex);
							vertex.textureCoordinate.x = uv[0];
							vertex.textureCoordinate.y = -uv[1];
						}
						break;
						default:
							break; // other reference modes not shown here!
						}
					}
					break;
					case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
					case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
					case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
						break;
					default:
						break;
					}
				}
				part.vertices.push_back(vertex);
				// Indices
				if (polyVertIndex >= 1 && polyVertIndex < vertexCount - 1) {
					// triangulate polygon
					
					part.indices.push_back(index + polyVertIndex + 1);
					part.indices.push_back(index + polyVertIndex);
					part.indices.push_back(index);
					
				}
			}
			index += vertexCount;
		}
		
		// assign a material
		auto elementMaterial = fbxMesh->GetElementMaterial();
		if (elementMaterial) {
			fbxsdk::FbxSurfaceMaterial* fbxMaterial = fbxMesh->GetNode()->GetMaterial(elementMaterial->GetIndexArray().GetAt(0));
			part.material = m_materials[fbxMaterial->GetName()];
			auto isAlpha = fbxMaterial->FindProperty("isalpha",false);
			if (isAlpha.IsValid() && isAlpha.Get<FbxBool>()) {
				part.alpha = true;
			}
		}
		
		mesh->AddPart(part);
		return mesh;
	}
	void CMF::ProcessCollisionNode(fbxsdk::FbxNode * node, CollisionModel & collision)
	{
		auto attribute = node->GetNodeAttribute();
		if (attribute) {
			auto type = attribute->GetAttributeType();
			// Attribute type determines how to process child nodes
			if (type == fbxsdk::FbxNodeAttribute::eMesh) {
				collision.hulls.push_back(CreateConvexHull((FbxMesh *)attribute));
			}
		}
		ProcessCollisionNodeChildren(node, collision);
	}
	void CMF::ProcessCollisionNodeChildren(fbxsdk::FbxNode * node, CollisionModel & collision)
	{
		for (int i = 0; i < node->GetChildCount(); i++) {
			ProcessCollisionNode(node->GetChild(i), collision);
		}
	}
	ConvexHull CMF::CreateConvexHull(fbxsdk::FbxMesh * fbxMesh)
	{
		ConvexHull hull;
		auto transform = fbxMesh->GetNode()->EvaluateGlobalTransform();
		// iterate control points
		for (int i = 0; i < fbxMesh->GetControlPointsCount(); i++) {
			auto control = transform.MultT(fbxMesh->GetControlPointAt(i));
			hull.AddVertex(Vector3(control[0], control[1], control[2]));
		}
		// iterate polygons
		for (int polygonIndex = 0; polygonIndex < fbxMesh->GetPolygonCount(); polygonIndex++) {
			
			fbxsdk::FbxVector4 axis;
			if (fbxMesh->GetPolygonVertexNormal(polygonIndex, 0, axis)) {
				axis = transform.MultT(axis) - transform.MultT(fbxsdk::FbxZeroVector4);
				hull.AddAxis(Vector3(axis[0], axis[1], axis[2]));
			}
		}
		return hull;
	}
	void CMF::ImportMaterials(fbxsdk::FbxScene * scene)
	{
		int numTabs = 0;
		std::ostringstream ss;
		for (int i = 0; i < scene->GetMaterialCount(); i++) {
			Material material;
			auto fbxMaterial = scene->GetMaterial(i);
			material.name = fbxMaterial->GetName();
			material.pixelShader = "C:\\Gage Omega\\Programming\\Procedural-RPG\\Assets\\" + fbxMaterial->ShadingModel.Get() + ".cso";
			
			//Get the implementation to see if it's a hardware shader.
			const fbxsdk::FbxImplementation* lImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_HLSL);
			FbxString lImplemenationType = "HLSL";
			if (!lImplementation)
			{
				lImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_CGFX);
				lImplemenationType = "CGFX";
			}
			if (!lImplementation)
			{
				lImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_SFX);
				lImplemenationType = "SFX";
			}
			if (!lImplementation)
			{
				lImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_OGS);
				lImplemenationType = "OGS";
			}
			if (!lImplementation)
			{
				lImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_MENTALRAY);
				lImplemenationType = "MENTALRAY";
			}
			if (!lImplementation)
			{
				lImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_PREVIEW);
				lImplemenationType = "PREVIEW";
			}
			if (!lImplementation)
			{
				lImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_NONE);
				lImplemenationType = "NONE";
			}
			if (lImplementation)
			{
				//Now we have a hardware shader, let's read it
				Utility::OutputLine(string("Hardware Shader Type: ") + lImplemenationType.Buffer());
				const FbxBindingTable* lRootTable = lImplementation->GetRootTable();
				FbxString lFileName = lRootTable->DescAbsoluteURL.Get();
				material.pixelShader = lFileName.Buffer();
				FbxString lTechniqueName = lRootTable->DescTAG.Get();
				size_t lEntryNum = lRootTable->GetEntryCount();
				for (int i = 0;i < (int)lEntryNum; ++i)
				{
					const FbxBindingTableEntry& lEntry = lRootTable->GetEntry(i);
					const char* lEntrySrcType = lEntry.GetEntryType(true);
					FbxProperty lFbxProp;
					FbxString lTest = lEntry.GetSource();
					Utility::OutputLine(string("Entry: ") + lTest.Buffer());
					if (strcmp(FbxPropertyEntryView::sEntryType, lEntrySrcType) == 0)
					{
						lFbxProp = fbxMaterial->FindPropertyHierarchical(lEntry.GetSource());
						if (!lFbxProp.IsValid())
						{
							lFbxProp = fbxMaterial->RootProperty.FindHierarchical(lEntry.GetSource());
						}
					}
					else if (strcmp(FbxConstantEntryView::sEntryType, lEntrySrcType) == 0)
					{
						lFbxProp = lImplementation->GetConstants().FindHierarchical(lEntry.GetSource());
					}
					if (lFbxProp.IsValid())
					{
						if (lFbxProp.GetSrcObjectCount<FbxTexture>() > 0)
						{
							//do what you want with the textures
							for (int j = 0; j < lFbxProp.GetSrcObjectCount<FbxFileTexture>(); ++j)
							{
								FbxFileTexture *lTex = lFbxProp.GetSrcObject<FbxFileTexture>(j);
								Utility::OutputLine(string("File Texture: ") + lTex->GetFileName());
								material.textures.push_back(lTex->GetFileName());
							}
							for (int j = 0; j < lFbxProp.GetSrcObjectCount<FbxLayeredTexture>(); ++j)
							{
								FbxLayeredTexture *lTex = lFbxProp.GetSrcObject<FbxLayeredTexture>(j);
								Utility::OutputLine(string("Layered Texture: ") + lTex->GetName());
							}
							for (int j = 0; j < lFbxProp.GetSrcObjectCount<FbxProceduralTexture>(); ++j)
							{
								FbxProceduralTexture *lTex = lFbxProp.GetSrcObject<FbxProceduralTexture>(j);
								Utility::OutputLine(string("Procedural Texture: ") + lTex->GetName());
							}
						}
					}
				}
			}
			else if (fbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
			{
				// We found a Phong material.  Display its properties.
				// Display the Ambient Color
				material.ambientColor = Convert(((FbxSurfacePhong *)fbxMaterial)->Ambient.Get());
				// Display the Diffuse Color
				material.diffuseColor = Convert(((FbxSurfacePhong *)fbxMaterial)->Diffuse.Get());
				// Display the Specular Color (unique to Phong materials)
				material.specularColor = Convert(((FbxSurfacePhong *)fbxMaterial)->Specular.Get());
				// Display the Emissive Color
				material.emissiveColor = Convert(((FbxSurfacePhong *)fbxMaterial)->Emissive.Get());
				//Opacity is Transparency factor now
				material.alpha = ((FbxSurfacePhong *)fbxMaterial)->TransparencyFactor.Get();
				// Display the Shininess
				material.specularPower = ((FbxSurfacePhong *)fbxMaterial)->Shininess;
				// Display the Reflectivity
				//((FbxSurfacePhong *)fbxMaterial)->ReflectionFactor;
			}
			else if (fbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
			{
				// We found a Lambert material. Display its properties.
				// Display the Ambient Color
				material.ambientColor = Convert(((FbxSurfaceLambert *)fbxMaterial)->Ambient.Get());
				// Display the Diffuse Color
				material.diffuseColor = Convert(((FbxSurfaceLambert *)fbxMaterial)->Diffuse.Get());
				// Display the Emissive
				material.emissiveColor = Convert(((FbxSurfaceLambert *)fbxMaterial)->Emissive.Get());
				// Display the Opacity
				material.alpha = (((FbxSurfaceLambert *)fbxMaterial)->TransparencyFactor.Get());
			}
			else
				ss << "Unknown type of Material" << std::endl;
			
			
			// Diffuse Textures
			vector<string> diffuseTextures = GetTextureConnections(fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse));
			if (diffuseTextures.size()) material.textures.push_back(diffuseTextures[0]);
			// Specular Textures
			vector<string> specularTextures = GetTextureConnections(fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular));
			if (specularTextures.size()) material.textures.push_back(specularTextures[0]);
			// Normal Textures
			vector<string> normalTextures = GetTextureConnections(fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap));
			if (normalTextures.size()) material.textures.push_back(normalTextures[0]);
			

			m_materials.insert(std::make_pair(material.name,std::make_shared<Material>(material)));

			// Debug output
			ss << "__________Material__________" << std::endl;

			ss << "Name : " << fbxMaterial->GetName() << std::endl;
			DisplayProperties(numTabs, ss, fbxMaterial);
			OutputDebugStringA(ss.str().c_str());
		}
	}
	void CMF::ImportTextures(fbxsdk::FbxScene * scene)
	{
		for (int i = 0; i < scene->GetTextureCount();i++) {
			auto texture = scene->GetTexture(i);
		}
	}
	XMFLOAT3 CMF::Convert(fbxsdk::FbxDouble3 & double3)
	{
		return XMFLOAT3(double3[0],double3[1],double3[2]);
	}
	vector<string> CMF::GetTextureConnections(FbxProperty & property)
	{
		vector<string> textures;
		//no layered texture simply get on the property
		int lNbTextures = property.GetSrcObjectCount<FbxTexture>();
		for (int j = 0; j < lNbTextures; ++j)
		{
			FbxFileTexture* lTexture = fbxsdk::FbxCast<FbxFileTexture>(property.GetSrcObject<FbxTexture>(j));
			if (lTexture)
			{
				auto path = Filesystem::proximate(lTexture->GetFileName(), "C:/Gage Omega/Programming/Procedural-RPG/Assets/");
				textures.push_back(path.string());
			}
		}
		return textures;
	}
	void CMF::Import(fbxsdk::FbxScene * scene)
	{
		// import textures
		ImportTextures(scene);

		// import materials
		ImportMaterials(scene);

		// import meshes recursively
		ProcessNode(scene->GetRootNode(), m_meshes);
	}
	void CMF::Export(Filesystem::path directory)
	{
		std::ofstream ofs(directory / m_name / ".cmf");
		Export(ofs);
	}
	shared_ptr<DirectX::Model> CMF::GetModel(ID3D11Device * d3dDevice, IEffectFactory * fxFactory, float distance)
	{
		// find the first lod group in which this distance fits within the threshhold
		for (auto & lodGroup : m_lodGroups) {
			if (distance <= lodGroup.GetThreshold()) {
				return lodGroup.GetModel(d3dDevice,fxFactory);
			}
		}
	}
	void CMF::Import(std::istream & ifs)
	{
	}
	void CMF::Export(std::ostream & ofs)
	{
	}
	/*bool CMF::TryGetAttribute(fbxsdk::FbxNode * node, int index, FbxNodeAttribute *& attribute)
	{
		attribute = node->GetNodeAttributeByIndex(index);
		return attribute != nullptr;
	}*/
}