#include "Component.h"
#include "Inventory.h"
class Resource :
	public Components::Component
{
public:
	Resource();
	Resource(vector<Components::InventoryItem> drops);
	// Data
	vector<Components::InventoryItem> Drops;
	// Inherited via Component
	virtual string GetName() override;
};
