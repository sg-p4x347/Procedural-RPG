#include "ShaderManager.h"


ShaderManager::ShaderManager()
{
}


ShaderManager::~ShaderManager() {
	delete colorShader;
	delete textureShader;
}

HRESULT ShaderManager::initialize(ID3D11Device* device, HWND hwnd) {
	HRESULT result = S_OK;

	// Create the color shader object.-------------------------------------
	colorShader = new ColorShader;

	// Initialize the color shader object.ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename
	result = colorShader->initialize(device, hwnd,L"color.vs",L"color.ps");
	if (FAILED(result)) {
		return result;
	}
	// Initialize the color shader object.ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename
	/*result = textureShader->initialize(device, hwnd);
	if (FAILED(result)) {
		return result;
	}*/

	// Create the texture shader object.-----------------------------------
	//textureShader = new TextureShader;

	//// Initialize the texture shader object.
	//result = textureShader->initialize(device, hwnd);
	//if (!result)
	//{
	//	return E_FAIL;
	//}

	return result;
}
HRESULT ShaderManager::RenderColorShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX * worldMatrix, XMMATRIX * viewMatrix, XMMATRIX * projectionMatrix) {
	return colorShader->render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix);
}
HRESULT ShaderManager::renderTextureShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX * worldMatrix, XMMATRIX * viewMatrix, XMMATRIX * projectionMatrix, ID3D11ShaderResourceView* texture) {
	return textureShader->render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}