#pragma once

#include "d3dclass.h"
#include "ColorShader.h"
#include "TextureShader.h"

class ShaderManager {
	public:
		ShaderManager();
		~ShaderManager();
		HRESULT initialize(ID3D11Device*, HWND);
		HRESULT RenderColorShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX * worldMatrix, XMMATRIX * viewMatrix, XMMATRIX * projectionMatrix);
		HRESULT renderTextureShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX * worldMatrix, XMMATRIX * viewMatrix, XMMATRIX * projectionMatrix, ID3D11ShaderResourceView* texture);
	private:
		ColorShader * colorShader;
		TextureShader * textureShader;
};

