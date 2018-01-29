#include "pch.h"
#include "WorldEntityManager.h"
// Concrete Component types
#include "Position.h"
#include "Player.h"
#include "PositionNormalTextureVBO.h"
#include "Movement.h"
#include "Tag.h"
#include "Terrain.h"
#include "Model.h"

WorldEntityManager::WorldEntityManager(Filesystem::path directory) : PersistenceEntityManager::PersistenceEntityManager(directory)
{
	//----------------------------------------------------------------
	// Register the components
	RegisterComponent([] {return new Components::Player();});

	RegisterComponent([] {return new Components::Position();});
	RegisterComponent([] {return new Components::Movement();});

	RegisterComponent([] {return new Components::Terrain();});

	RegisterComponent([] {return new Components::PositionNormalTextureVBO();});
	RegisterComponent([] {return new Components::Model();});

	//----------------------------------------------------------------
	// Tags
	RegisterDelegate([](string type) {return new Components::Tag(type);}, vector<string>{
		"Water",
		"Tree"
	});
}
