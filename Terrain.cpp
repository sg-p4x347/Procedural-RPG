#include "pch.h"
#include "Terrain.h"

namespace Components {
	

	Terrain::Terrain(const Terrain & other) : Terrain::Terrain()
	{
	}

	Terrain::Terrain()
	{
	}

	string Terrain::GetName()
	{
		return "Terrain";
	}
}