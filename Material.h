#pragma once
#include "ISerialization.h"
namespace geometry {
	struct Material : public ISerialization
	{
		Material();
		string name;
		float specularPower;
		bool perVertexColor;
		bool enableSkinning;
		float alpha;
		XMFLOAT3 ambientColor;
		XMFLOAT3 diffuseColor;
		XMFLOAT3 specularColor;
		XMFLOAT3 emissiveColor;
		vector<string> textures;
		string pixelShader;

		shared_ptr<DirectX::IEffect> GetEffect(IEffectFactory * fxFactory);

		// Inherited via ISerialization
		virtual void Import(std::istream & ifs) override;
		virtual void Export(std::ostream & ofs) override;
	};
}
