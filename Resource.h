#include "WorldComponent.h"
#include "Inventory.h"
namespace world{
class Resource :
	public WorldComponent
{
public:
	Resource();
	Resource(vector<InventoryItem> drops);
	// Data
	vector<InventoryItem> Drops;
};
}
