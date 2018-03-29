#include "pch.h"
#include "Resource.h"


Resource::Resource()
{
}

Resource::Resource(vector<Components::InventoryItem> drops) : Drops(drops)
{

}

string Resource::GetName()
{
	return "Resource";
}
