#include "pch.h"
#include "Entity.h"

Entity::Entity(vector<string> & components)
{
	Components = components;
}

void Entity::Import(JsonParser & jp)
{
	ID = (unsigned int)jp["id"];
	for (JsonParser & comp : jp["comps"].GetElements()) {
		Components.push_back((string)comp);
	}
}

JsonParser Entity::Export()
{
	JsonParser obj;
	obj.Set("id",ID);
	JsonParser comps;
	for (string & component : Components) {
		comps.Add(component);
	}
	obj.Set("comps", comps);
	return obj;
}
