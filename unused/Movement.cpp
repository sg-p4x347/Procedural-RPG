#include "pch.h"
#include "Movement.h"


Movement::Movement()
{
}

shared_ptr<Component> Movement::GetComponent(const unsigned int & id)
{
	return shared_ptr<Component>();
}

void Movement::Attach(const unsigned int & id)
{
}

void Movement::Save(string directory)
{
}

void Movement::Load(string directory)
{
}

