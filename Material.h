#pragma once
namespace geometry {
	struct Material
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
	};
}
