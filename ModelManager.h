#pragma once

using namespace DirectX;
using string = std::string;

class ModelManager
{
public:
	static ModelManager & Instance();
	// .cmo contains mesh, texture, and material properties
	Model GetCMO(Microsoft::WRL::ComPtr<ID3D11Device> device, unsigned short modelID, unsigned short LODlevel = 0);
	// .vbo contains mesh
	Model GetVBO(Microsoft::WRL::ComPtr<ID3D11Device> device, unsigned short modelID, unsigned short LODlevel = 0);
private:
	ModelManager();

	std::vector<std::unique_ptr<Model>> m_models;

	// Private methods
	bool GetModel(std::wstring filename, Model & modelOut);
	void AddModel(std::unique_ptr<Model> & model,std::wstring filename);
	std::wstring GetFilename(unsigned short modelID, unsigned short LODlevel, std::wstring extension);
};

