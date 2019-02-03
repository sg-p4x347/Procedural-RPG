#include "pch.h"
#include "Material.h"

namespace geometry {
	Material::Material(string name) :
		name(name),
		specularPower(0.f),
		perVertexColor(false),
		enableSkinning(false),
		alpha(1.f),
		ambientColor(1,1,1),
		diffuseColor(1,1,1),
		specularColor(),
		emissiveColor(),
		textures(),
		pixelShader("Lambert.cso")
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
			info.ambientColor = ambientColor;
			info.diffuseColor = diffuseColor;
			info.specularColor = specularColor;
			info.emissiveColor = emissiveColor;
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
	void Material::Import(std::istream & ifs)
	{
		DeSerialize(name, ifs);
		DeSerialize(specularPower, ifs);
		DeSerialize(perVertexColor, ifs);
		DeSerialize(enableSkinning, ifs);
		DeSerialize(alpha, ifs);
		DeSerialize(ambientColor, ifs);
		DeSerialize(diffuseColor, ifs);
		DeSerialize(specularColor, ifs);
		DeSerialize(emissiveColor, ifs);
		size_t textureCount = 0;
		DeSerialize(textureCount, ifs);
		for (size_t textureIndex = 0; textureIndex < textureCount; textureIndex++) {
			string texture;
			DeSerialize(texture, ifs);
			textures.push_back(texture);
		}
		DeSerialize(pixelShader, ifs);
	}
	void Material::Export(std::ostream & ofs)
	{
		Serialize(name, ofs);
		Serialize(specularPower, ofs);
		Serialize(perVertexColor, ofs);
		Serialize(enableSkinning, ofs);
		Serialize(alpha, ofs);
		Serialize(ambientColor, ofs);
		Serialize(diffuseColor, ofs);
		Serialize(specularColor, ofs);
		Serialize(emissiveColor, ofs);
		Serialize(textures.size(), ofs);
		for (auto & texture : textures) {
			Serialize(texture, ofs);
		}

		Serialize(pixelShader, ofs);
	}
}