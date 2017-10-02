#include "pch.h"
#include "ModelManager.h"

using namespace DirectX;

ModelManager & ModelManager::Instance()
{
	static ModelManager manager;
	return manager;
}

Model ModelManager::GetCMO(Microsoft::WRL::ComPtr<ID3D11Device> device, unsigned short modelID, unsigned short LODlevel)
{
	std::wstring filename = GetFilename(modelID,LODlevel,L".cmo");
	Model model;
	if (GetModel(filename, model)) {
		return model;
	} else {
		DGSLEffectFactory fx(device.Get());
		// Can also use EffectFactory, but will ignore pixel shader material settings
		std::unique_ptr<Model> uniqueModel = Model::CreateFromCMO(device.Get(), filename.c_str(),fx);
		AddModel(uniqueModel, filename);
		return *uniqueModel;
	}
}

Model ModelManager::GetVBO(Microsoft::WRL::ComPtr<ID3D11Device> device, unsigned short modelID, unsigned short LODlevel)
{
	std::wstring filename = GetFilename(modelID, LODlevel, L".vbo");
	Model model;
	if (GetModel(filename, model)) {
		return model;
	} else {
		std::unique_ptr<Model> uniqueModel = Model::CreateFromVBO(device.Get(), filename.c_str());
		AddModel(uniqueModel, filename);
		return *uniqueModel;
	}
}

ModelManager::ModelManager()
{

}

bool ModelManager::GetModel(std::wstring filename, Model & modelOut)
{
	// figure out if the model is already loaded
	for (std::unique_ptr<Model> & model : m_models) {
		if (model->name == filename) {
			// found a match
			modelOut = Model(*model);
			return true;
		}
	}
	return false;
}

void ModelManager::AddModel(std::unique_ptr<Model> & model, std::wstring filename)
{
	model->name = filename;
}

std::wstring ModelManager::GetFilename(unsigned short modelID, unsigned short LODlevel, std::wstring extension)
{
	return std::to_wstring(modelID) + L"_" + std::to_wstring(LODlevel) + L"." + extension;
}
