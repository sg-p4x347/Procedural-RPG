#include "pch.h"
#include "Terrain.h"

namespace Components {
	

	Terrain::Terrain(const Terrain & other) : Terrain::Terrain(other.ID)
	{
	}

	Terrain::Terrain(const unsigned int & id) : Component::Component(id)
	{
	}

	string Terrain::GetName()
	{
		return "Terrain";
	}
}