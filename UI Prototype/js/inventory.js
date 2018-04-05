// Developer: Gage Coates
// Purpose: UI prototyping for the Procedural-RPG project

function InventoryItem(name, description, quantity) {
	this.Name = name;
	this.Description = description;
	this.Quantity = quantity;
}
var selectedItems = [];

function ToggleItem(itemElement) {
	var found = selectedItems.some(function(selectedItem,index) {
		if (selectedItem == itemElement) {
			selectedItems.splice(index,1);
			return true;
		}
	});
	if (!found) {
		itemElement.classList.add("item-selected");
		selectedItems.push(itemElement);
	} else {
		itemElement.classList.remove("item-selected");
	}
}

function SelectTab(tabElement) {
	var children = document.getElementById("Tabs").children;
	for (var i = 0; i < children.length; i++) {
		children[i].classList.remove("item-selected");
	}
	tabElement.classList.add("item-selected");
}
