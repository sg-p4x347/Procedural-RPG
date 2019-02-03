#include "pch.h"
#include "SoundAsset.h"


SoundAsset::SoundAsset()
{
}


string SoundAsset::GetName()
{
	return "SoundAsset";
}

void SoundAsset::Import(std::istream & ifs)
{
	JsonParser jp(ifs);
	Import(jp);
}

void SoundAsset::Export(std::ostream & ofs)
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
