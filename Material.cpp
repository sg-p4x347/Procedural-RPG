#include "pch.h"
#include "Material.h"

namespace geometry {
	Material::Material() :
		name("Default"),
		specularPower(0.f),
		perVertexColor(true),
		enableSkinning(false),
		alpha(1.f),
		ambientColor(1,1,1),
		diffuseColor(1,1,1),
		specularColor(),
		emissiveColor(),
		textures(),
		pixelShader()
	{
	}
	shared_ptr<DirectX::IEffect> Material::GetEffect(IEffectFactory * fxFactory)
	{
		auto fxFactoryDGSL = dynamic_cast<DGSLEffectFactory*>(fxFactory);
		if (fxFactoryDGSL)
		{
			DGSLEffectFactory::DGSLEffectInfo info;
			info.name =  ansi2unicode(name).c_str();
			info.specularPower = specularPower;
			info.perVertexColor = perVertexColor;
			info.enableSkinning = enableSkinning;
			info.alpha = alpha;
			info.ambientColor = ambientColor * 255.f;
			info.diffuseColor = diffuseColor * 255.f;
			info.specularColor = specularColor;
			info.emissiveColor = emissiveColor * 255.f;
			vector<wstring> wideTextures;
			for (auto & texture : textures) {
				wideTextures.push_back(ansi2unicode(texture));
			}
			info.diffuseTexture = wideTextures.size() >= 1 ? wideTextures[0].c_str() : nullptr;
			info.specularTexture = wideTextures.size() >= 2 ? wideTextures[1].c_str() : nullptr;
			info.normalTexture = wideTextures.size() >= 3 ? wideTextures[2].c_str() : nullptr;
			wstring pShaderWide = ansi2unicode(pixelShader);
			info.pixelShader = pShaderWide.c_str();

			const int offset = DGSLEffectFactory::DGSLEffectInfo::BaseTextureOffset;
			for (int i = 0; i < (DGSLEffect::MaxTextures - offset); ++i)
			{
				info.textures[i] = wideTextures.size() >= i + offset + 1 ? wideTextures[i + offset].c_str() : nullptr;
			}

			return fxFactoryDGSL->CreateDGSLEffect(info, nullptr);

			//auto dgslEffect = static_cast<DGSLEffect*>(m.effect.get());
			//dgslEffect->SetUVTransform(XMLoadFloat4x4(&m.pMaterial->UVTransform));
		}
		return nullptr;
	}
}