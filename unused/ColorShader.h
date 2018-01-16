#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

class ColorShader{
	public:
		ColorShader();
		~ColorShader();
		HRESULT initialize(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
		HRESULT render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX * worldMatrix, XMMATRIX * viewMatrix, XMMATRIX * projectionMatrix);
		HRESULT setShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX * worldMatrix, XMMATRIX * viewMatrix, XMMATRIX * projectionMatrix);
	private:
		void outputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);
		HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
		struct MatrixBufferType
		{
			XMMATRIX world;
			XMMATRIX view;
			XMMATRIX projection;
		};
		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;
		ID3D11InputLayout* layout;
		ID3D11Buffer* matrixBuffer;
};
