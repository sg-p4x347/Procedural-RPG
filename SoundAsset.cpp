#include "pch.h"
#include "SoundAsset.h"


SoundAsset::SoundAsset()
{
}


string SoundAsset::GetName()
{
	return "SoundAsset";
}

void SoundAsset::Import(std::ifstream & ifs)
{
	JsonParser jp(ifs);
	Import(jp);
}

void SoundAsset::Export(std::ofstream & ofs)
{
	Export().Export(ofs);
}

void SoundAsset::Import(JsonParser & jp)
{
}

JsonParser SoundAsset::Export()
{
	JsonParser jp;
	return jp;
}
