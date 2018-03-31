#pragma once
#include "System.h"
#include "GuiEntityManager.h"
#include "Sprite.h"
#include "GuiText.h"
#include "Inventory.h"
using namespace GUI;
class SystemManager;
class GuiSystem :
	public System
{
public:
	GuiSystem(SystemManager * systemManager,
		unsigned short updatePeriod);
	~GuiSystem();

	// Inherited via System
	virtual void SyncEntities() override;
	virtual string Name() override;
	virtual void Update(double & elapsed) override;
	// Window resize update
	void UpdateUI(int outputWidth, int outputHeight);

	void OpenMenu(string name);
	EntityPtr GetCurrentMenu();
	void CloseMenu();
	GuiEntityManager & GetEM();
	void DisplayException(std::exception e);
	//----------------------------------------------------------------
	// HUD Control
	void ShowHint(string hint);
	void HideHint();
	//----------------------------------------------------------------
	// Events
	void BindHandlers();
	void CharTyped(char ch);
	void Backspace();
private:
	SystemManager * SM;
	GUI::GuiEntityManager GuiEM;
	Rectangle m_outputRect;
	//----------------------------------------------------------------
	// Menus
	EntityPtr m_currentMenu;
	string m_currentMenuName;
	map<string, EntityPtr> m_menus;
	map<string, std::function<EntityPtr(void)>> m_dynamicMenus;
	void AddMenu(string name, EntityPtr menu);
	void AddDynamicMenu(string name, std::function<EntityPtr(void)> constructor);
	EntityPtr GetMenu(string name);
	void OpenMenu(EntityPtr menu);
	bool IsMenuOpen(string name);
	//----------------------------------------------------------------
	// Menu factories
	EntityPtr CreateInventory();
	EntityPtr CreateInventoryGrid(vector<Components::InventoryItem> inventory);
	void SelectInventoryTab(EntityPtr gridContainer, string category);
	//----------------------------------------------------------------
	// HUD
	EntityPtr m_HUDhint;

	//----------------------------------------------------------------
	// Spritesheets
	map<string, map<string, Rectangle>> m_spriteSheets;
	void AddSpriteSheet(string path, map<string, Rectangle> mapping);
	Rectangle GetSpriteRect(string filePath, string spriteName);
	//----------------------------------------------------------------
	// Style Updates
	EntityPtr m_activeElement;
	EntityPtr m_hoverElement;
	void UpdateSprite(EntityPtr entity, shared_ptr<Style> style, shared_ptr<Sprite> sprite, int zIndex);
	void UpdateText(shared_ptr<Text> text, shared_ptr<Sprite> sprite, shared_ptr<Style> style);

	void SetStyle(EntityPtr entity, Style style);
	//----------------------------------------------------------------
	// Events

	void OnHover(EntityPtr entity, Event evt);
	void OnHoverOut(EntityPtr entity, Event evt);
	void OnMouseDown(EntityPtr entity, Event evt);
	void OnMouseUp(EntityPtr entity, Event evt);
	void OnClick(EntityPtr entity, Event evt);
	void OnDrag(EntityPtr entity, Event evt);
	void OnScroll(EntityPtr entity, Event evt);
	void OnKeydown(EntityPtr entity, Event evt);

	void OnDefault(EntityPtr entity);
	//----------------------------------------------------------------
	// User Interface
	const int m_scrollTicks;
	/* Iterate recursively into the UI heierarchy to locate the lowest level
	element that the vector occupies*/
	EntityPtr FindOccupiedRecursive(EntityPtr entity, Vector2 mousePos);
	EntityPtr FindFirstParent(EntityPtr child, std::function<bool(shared_ptr<Style>)> && validation = [](shared_ptr<Style> style) {return true;});
	//----------------------------------------------------------------
	// View engine
	void UpdateFlowRecursive(EntityPtr entity, int zIndex);
	void PositionChildren(EntityPtr parent);

	Rectangle CalculateChildRect(Rectangle parentRect, shared_ptr<Style> childStyle);
	void SetPosition(FlowType flow, Rectangle & rect, int primary, int secondary);

	int GetPrimaryDimension(FlowType flow, Rectangle rect);
	int GetSecondaryDimension(FlowType flow, Rectangle rect);

	int GetPrimaryPosition(FlowType flow, Rectangle rect);
	int GetSecondaryPosition(FlowType flow, Rectangle rect);

	Vector2 GetVector(FlowType flow, int primary, int secondary);
	// factories
	EntityPtr NewVerticalScrollBar(EntityPtr target);
	EntityPtr MainMenuBtn(int height = 100);
	EntityPtr NewTextBox(string id,string placeholder = "");
	//----------------------------------------------------------------
	// Misc helpers
	void DeleteChildren(EntityPtr & parent);
	void DeleteRecursive(EntityPtr & parent);
	void ReplaceChildren(EntityPtr parent, EntityPtr child);
	shared_ptr<Sprite> GetSprite(EntityPtr entity);
	shared_ptr<Style> GetStyle(EntityPtr entity);
	void AddRectIfValid(Rectangle rect, vector<Rectangle> & rects);
	void AddScrollbarsRecursive(EntityPtr & entity);
	EntityPtr GetElementByID(string id);
	EntityPtr FindElementByIdRecursive(EntityPtr entity, string id);
};

