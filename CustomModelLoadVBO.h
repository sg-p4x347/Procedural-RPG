#pragma once
namespace VBO
{
	struct header_t
	{
		uint32_t numVertices;
		uint32_t numIndices;
	};

}; // namespace

static_assert(sizeof(VBO::header_t) == 8, "VBO header size mismatch");
class CustomModelLoadVBO
{
public:
	// Loads a model from a .VBO file
	static std::shared_ptr<Model> __cdecl CreateFromVBO(_In_ ID3D11Device* d3dDevice, _In_reads_bytes_(dataSize) const uint8_t* meshData, _In_ size_t dataSize,
		_In_opt_ std::shared_ptr<IEffect> ieffect = nullptr, bool ccw = false, bool pmalpha = false);
	static std::shared_ptr<Model> __cdecl CreateFromVBO(_In_ ID3D11Device* d3dDevice, string fileName,
		_In_opt_ std::shared_ptr<IEffect> ieffect = nullptr, bool ccw = false, bool pmalpha = false);
	// Converts a VBO component into a Model
	static std::shared_ptr<Model> __cdecl CreateFromVBO(_In_ ID3D11Device* d3dDevice, vector<VertexPositionNormalTangentColorTexture> & vertices, vector<uint16_t> & indices,
		_In_opt_ std::shared_ptr<IEffect> ieffect = nullptr, bool ccw = false, bool pmalpha = false);
};

