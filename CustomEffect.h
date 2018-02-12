#pragma once
#include "Effects.h"
struct ObjectConstants
{
	XMMATRIX    LocalToWorld4x4;
	XMMATRIX    LocalToProjected4x4;
	XMMATRIX    WorldToLocal4x4;
	XMMATRIX    WorldToView4x4;
	XMMATRIX    UvTransform4x4;
	XMVECTOR    EyePosition;
};
class CustomEffect :
	public BasicEffect
{
public:
	CustomEffect(ID3D11Device * device, Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader, Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader);
	void __cdecl Apply(_In_ ID3D11DeviceContext* deviceContext) override;
	void SetTexture(const UINT index, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture);
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textures[8];
};

