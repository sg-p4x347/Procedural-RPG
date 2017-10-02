#pragma once

using string = std::string;

class ModelManager
{
public:
	static ModelManager & Instance();
	// .cmo contains mesh, texture, and material properties
	DirectX::Model GetCMO(Microsoft::WRL::ComPtr<ID3D11Device> device, unsigned short modelID, unsigned short LODlevel = 0);
	// .vbo contains mesh
	DirectX::Model GetVBO(Microsoft::WRL::ComPtr<ID3D11Device> device, unsigned short modelID, unsigned short LODlevel = 0);
private:
	ModelManager();

	std::vector<std::unique_ptr<DirectX::Model>> m_models;

	// Private methods
	bool GetModel(std::wstring filename, DirectX::Model & modelOut);
	void AddModel(std::unique_ptr<DirectX::Model> & model,std::wstring filename);
	std::wstring GetFilename(unsigned short modelID, unsigned short LODlevel, std::wstring extension);
};

