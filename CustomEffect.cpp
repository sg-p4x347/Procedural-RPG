#include "pch.h"
#include "CustomEffect.h"



CustomEffect::CustomEffect(ID3D11Device * device, Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader, Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader) :
	BasicEffect::BasicEffect(device), 
	m_pixelShader(pixelShader),
	m_vertexShader(vertexShader)
{

}

void CustomEffect::Apply(ID3D11DeviceContext * deviceContext)
{
	BasicEffect::Apply(deviceContext);
	// Make sure the constant buffers are up to date.
	//-----------------------------
	deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	// Textures
	ID3D11ShaderResourceView* txt[8] = { m_textures[0].Get(), m_textures[1].Get(), m_textures[2].Get(), m_textures[3].Get(),
		m_textures[4].Get(), m_textures[5].Get(), m_textures[6].Get(), m_textures[7].Get() };
	deviceContext->PSSetShaderResources(0, 8, txt);
}

void CustomEffect::SetTexture(const UINT index, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture)
{
	m_textures[index] = texture;
}
