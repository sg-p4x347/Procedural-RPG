#include "pch.h"
#include "Flag.h"


Flag::Flag(string flagName) : Component(flagName)
{

}


Flag::~Flag()
{
}

shared_ptr<Component> Flag::GetComponent(const unsigned int & id)
{
	return shared_ptr<Component>();
}

void Flag::Attach(const unsigned int & id)
{
}

void Flag::Save(string directory)
{
}

void Flag::Load(string directory)
{
}
