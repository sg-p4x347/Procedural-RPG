#include "pch.h"
#include "CMF.h"
#include <fbxsdk.h>
namespace geometry {
	CMF::CMF()
	{
	}


	CMF::~CMF()
	{
	}
	shared_ptr<CMF> CMF::CreateFromFBX(string path)
	{
		// Change the following filename to a suitable filename value.
		const char* lFilename = path.c_str();

		// Initialize the SDK manager. This object handles memory management.
		FbxManager* lSdkManager = FbxManager::Create();

		// Create the IO settings object.
		FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
		lSdkManager->SetIOSettings(ios);

		// Create an importer using the SDK manager.
		FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

		// Use the first argument as the filename for the importer.
		if (!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings())) {
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
			exit(-1);
		}
		// Create a new scene so that it can be populated by the imported file.
		FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");

		// Import the contents of the file into the scene.
		lImporter->Import(lScene);

		// The file is imported, so get rid of the importer.
		lImporter->Destroy();

		// Print the nodes of the scene and their attributes recursively.
		// Note that we are not printing the root node because it should
		// not contain any attributes.
		FbxNode* lRootNode = lScene->GetRootNode();
		if (lRootNode) {
			for (int i = 0; i < lRootNode->GetChildCount(); i++) {
				auto child = lRootNode->GetChild(i);
				for (int i = 0; i < child->GetNodeAttributeCount(); i++) {
					FbxNodeAttribute* pAttribute = child->GetNodeAttributeByIndex(i);
					if (!pAttribute) continue;
					string typeName = [=] {
						switch (pAttribute->GetAttributeType()) {
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
					}();
					FbxString attrName = pAttribute->GetName();
				}
			}
		}
		// Destroy the SDK manager and all the other objects it was handling.
		lSdkManager->Destroy();
		return nullptr;
	}
}