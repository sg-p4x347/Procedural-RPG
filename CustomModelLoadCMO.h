#pragma once
class CustomModelLoadCMO
{
public:
	static std::unique_ptr<Model> __cdecl CreateFromCMO(_In_ ID3D11Device* d3dDevice, _In_reads_bytes_(dataSize) const uint8_t* meshData, size_t dataSize,
		_In_ IEffectFactory& fxFactory, bool ccw = true, bool pmalpha = false);
	static std::unique_ptr<Model> __cdecl CreateFromCMO(_In_ ID3D11Device* d3dDevice, _In_z_ const wchar_t* szFileName,
		_In_ IEffectFactory& fxFactory, bool ccw = true, bool pmalpha = false);
};

