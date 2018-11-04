#include "pch.h"
#include "SystemManager.h"

#include "GuiSystem.h"
#include "ItemSystem.h"
#include "GuiHandler.h"

#include "GuiStyle.h"
#include "GuiPanel.h"
#include "GuiChildren.h"
#include "GuiEventHandler.h"
#include "Game.h"
#include "GuiText.h"
#include "AssetManager.h"
#include "IEventManager.h"
#include "World.h"
#include "XmlParser.h"
#include "CssParser.h"
//#include "InventoryGuiHandler.h"

using namespace GUI;
using namespace DirectX;
using ButtonState = DirectX::Mouse::ButtonStateTracker::ButtonState;

GuiSystem::GuiSystem(
	SystemManager * systemManager,
	unsigned short updatePeriod
) : System::System(updatePeriod,false),
m_scrollTicks(1200),
SM(systemManager)
{
	//----------------------------------------------------------------
	// Initialize spritesheets
	AddSpriteSheet("widget.png", std::map<string, Rectangle>{
		{ "button", Rectangle(0, 0, 384, 128)},
		{ "button_hover",Rectangle(0,128,384,128) },
		{ "button_active",Rectangle(0,256,384,128) },

		{ "bigradient", Rectangle(0, 384, 384, 128) },
		{ "bigradient_hover",Rectangle(0,512,384,128) },
		{ "bigradient_active",Rectangle(0,640,384,128) },

		{ "scroll_handle",Rectangle(384,32,32,128) },
		{ "scroll_handle_hover",Rectangle(416,32,32,128) },
		{ "scroll_handle_active",Rectangle(448,32,32,128) }
	});

	//----------------------------------------------------------------
	// Common styles
	// Generate the entire GUI

	//----------------------------------------------------------------
	// main
#pragma region main
	AddMenu("main", GuiEM.NewPanel([] {
		Style * style = new Style();
		style->Background = "main.png";
		return style;
	}(), vector<EntityPtr>{
		// Header
		[=] {
			auto panel = GuiEM.NewPanel([] {
				Style * style = new Style();
				style->Background = "rgba(1,1,1,1)";
				style->FontSize = "128px";
				style->FontColor = "rgb(0,0,0)";
				style->TextAlign = "center";
				style->Height = "15%";
				return style;
			}());
			GuiEM.AddText(panel, "Procedural RPG");
			return panel;
		}(),
			// Side panel
			GuiEM.NewPanel([] {
			Style * style = new Style();
			style->Justify = "center";
			style->Height = "85%";
			style->Width = "20%";
			return style;
		}(), vector<EntityPtr>{
			GuiEM.NewButton("Continue", [=](Event evt) {
				Game::Get().LoadWorld();
			}),
			GuiEM.NewButton("New World", [=](Event evt) {
				OpenMenu("new_world");
			}),
			GuiEM.NewButton("Load World", [=](Event evt) {
				OpenMenu("load_world");
			}),
			GuiEM.NewButton("Options", [=](Event evt) {
				OpenMenu("options");
			})
		})
	}));
		
#pragma endregion
	//----------------------------------------------------------------
	// game_paused
#pragma region game_paused
	AddMenu("game_paused", GuiEM.NewPanel([] {
		Style * style = new Style();
		return style;
	}(), vector<EntityPtr>{
		// Header
		[=] {
			auto panel = GuiEM.NewPanel([] {
				Style * style = new Style();
				style->Background = "rgba(1,1,1,0.5)";
				style->FontSize = "128px";
				style->TextAlign = "center";
				style->Height = "15%";
				return style;
			}());
			GuiEM.AddText(panel, "Game Paused");
			return panel;
		}(),
			// Side panel
			GuiEM.NewPanel([] {
			Style * style = new Style();
			style->Justify = "center";
			style->Height = "85%";
			style->Width = "20%";
			return style;
		}(), vector<EntityPtr>{
			GuiEM.NewButton("Resume Game", [=](Event evt) {
				world::World * world = nullptr;
				if (Game::Get().TryGetWorld(world)) {
					world->ResumeGame();
				}
			}),
			MainMenuBtn()
		})
	}));
#pragma endregion
	//----------------------------------------------------------------
	// HUD
#pragma region HUD
	AddMenu("HUD", ImportMarkup("UI/hud.xml"));
	/*m_HUDhint = GuiEM.NewTextPanel("Hint", [] {
		Style * style = new Style();
		style->FontSize = "32px";
		style->TextAlign = "center";
		style->FontColor = "rgb(1,1,1)";
		style->VerticalTextAlign = "center";
		style->Height = "100%";
		return style;
	}());*/
	
	//m_HUDhint->GetComponent<GUI::Panel>("Panel")->ElementID = "HUDhint";
	//AddMenu("HUD", m_HUDhint);
#pragma endregion
	//----------------------------------------------------------------
	// Inventory
#pragma region Inventory
	//AddDynamicMenu("inventory", [this] {return CreateInventory();});
#pragma endregion
	//----------------------------------------------------------------
	// Options
#pragma region options
	AddMenu("options", GuiEM.NewPanel([] {
		Style * style = new Style();
		return style;
	}(), vector<EntityPtr>{
		// Header
		[=] {
			auto panel = GuiEM.NewPanel([] {
				Style * style = new Style();
				style->Background = "rgba(1,1,1,0.5)";
				style->FontSize = "128px";
				style->TextAlign = "center";
				style->Height = "15%";
				return style;
			}());
			GuiEM.AddText(panel, "Options");
			return panel;
		}(),
			// Side panel
			GuiEM.NewPanel([] {
			Style * style = new Style();
			style->Justify = "center";
			style->Height = "85%";
			style->Width = "20%";
			return style;
		}(), vector<EntityPtr>{
			GuiEM.NewButton("Controls", [=] (Event evt){
			}),
			MainMenuBtn()
		})
	}));
#pragma endregion
	//----------------------------------------------------------------
	// New World
#pragma region new_world
	auto nameTextbox = NewTextBox("WorldName", "World1");
	auto seedTextbox = NewTextBox("WorldSeed", "World Seed");
	AddMenu("new_world", GuiEM.NewPanel([] {
		Style * style = new Style();
		return style;
	}(), vector<EntityPtr>{
		// Header
		[=] {
			auto panel = GuiEM.NewPanel([] {
				Style * style = new Style();
				style->Background = "rgba(1,1,1,0.5)";
				style->FontSize = "128px";
				style->TextAlign = "center";
				style->Height = "20%";
				return style;
			}());
			GuiEM.AddText(panel, "Create a world");
			return panel;
		}(),
		GuiEM.NewPanel([] {
			Style * style = new Style();
			style->FlowDirection = "row";
			style->Width = "100%";
			style->Height = "80%";
			return style;
		}(), vector<EntityPtr>{
			// Side panel
			GuiEM.NewPanel([] {
				Style * style = new Style();
				style->Height = "100%";
				style->Width = "20%";
				return style;
			}(), vector<EntityPtr>{
				MainMenuBtn()
			}),
			// Body
			GuiEM.NewPanel([] {
				Style * style = new Style();
				style->Justify = "center";
				style->AlignItems = "center";
				style->Height = "100%";
				style->Width = "80%";
				return style;
			}(), vector<EntityPtr>{
				nameTextbox,
				seedTextbox,
				[=] {
					auto btn = GuiEM.NewButton("Generate!", [=](Event evt) {
						auto name = nameTextbox->GetComponent<Text>("Text");
						
						if (name && name->String.length() > 0) {
							// parse seed
							string seedText = seedTextbox->GetComponent<Text>("Text")->String;
							// valid

							Game::Get().GenerateWorld(name->String,ProUtil::ToRandom(seedText));
							
						}
						else {
							// invalid name

						}
					},"widget.png:bigradient","widget.png:bigradient_hover","widget.png:bigradient_active");
					auto style = btn->GetComponent<Style>("Style_Default");
					style->TextAlign = "center";
					return btn;
				}()
			})
		})
	}));
#pragma endregion
	//----------------------------------------------------------------
	// Load World
#pragma region load_world
	AddDynamicMenu("load_world", [=] {
		vector<EntityPtr> worldButtons;
		for (auto & dir : Filesystem::directory_iterator(Game::Get().GetSavesDirectory())) {
			string folderName = dir.path().filename().string();
			auto button = GuiEM.NewButton(folderName, [=](Event evt) {
				Game::Get().LoadWorld(folderName);
			}, "rgb(0.5,0.5,0.5)", "rgb(0.75,0.75,0.75)", "rgb(0,0,0.1)");
			/*auto defaultStyle = button->GetComponent<Style>("Style_Default");
			defaultStyle->*/
			worldButtons.push_back(button);
		}

		return GuiEM.NewPanel([=] {
			Style * style = new Style();
			style->FlowDirection = "row";
			return style;
		}(), vector<EntityPtr>{
			// Side panel
			GuiEM.NewPanel([] {
				Style * style = new Style();
				style->Background = "rgba(1,1,1,0.5)";
				style->FontSize = "128px";
				style->TextAlign = "center";
				style->Height = "100%";
				style->Width = "20%";
				style->AlignItems = "start";
				return style;
			}(), vector<EntityPtr>{
				MainMenuBtn(64)
			}),
				// List panel
				GuiEM.NewPanel([] {
				Style * style = new Style();
				style->FlowDirection = "column";
				style->Justify = "start";
				style->AlignItems = "center";
				style->Height = "100%";
				style->Width = "80%";
				style->OverflowY = "scroll";
				return style;
			}(), worldButtons)
		});
	});
#pragma endregion
	//----------------------------------------------------------------
	// Controls
#pragma region controls
	AddMenu("controls", GuiEM.NewPanel([] {
		Style * style = new Style();
		return style;
	}(), vector<EntityPtr>{
		// Body
		[=] {
			auto panel = GuiEM.NewPanel([] {
				Style * style = new Style();
				style->Background = "rgba(1,1,1,0.5)";
				style->FontSize = "128px";
				style->TextAlign = "center";
				style->Height = "100%";
				style->Width = "80%";
				return style;
			}());
			GuiEM.AddText(panel, "Controls");
			return panel;
		}(),
		// Side panel
		GuiEM.NewPanel([] {
			Style * style = new Style();
			style->Justify = "center";
			style->Height = "100%";
			style->Width = "20%";
			return style;
		}(), vector<EntityPtr>{
			
		})
	}));
#pragma endregion
#pragma region loading
	AddMenu("loading", ImportMarkup("UI/loading.xml"));
#pragma endregion
}


GuiSystem::~GuiSystem()
{
}

void GuiSystem::RegisterSubsystems()
{
	//----------------------------------------------------------------
	// Initialize sub-systems
	/*auto inventory = new InventoryGuiHandler(SM, GetEM());
	inventory->RegisterHandlers();
	SM->AddSystem(std::shared_ptr<System>(inventory));*/
}

void GuiSystem::SyncEntities()
{
	
}

string GuiSystem::Name()
{
	return "Gui";
}

void GuiSystem::Update(double & elapsed)
{
	if (m_currentMenu) {
		if (Game::MouseState.positionMode == Mouse::MODE_ABSOLUTE) {
			Vector2 mousePos(Game::MouseState.x, Game::MouseState.y);
			int scrollTicks = DirectX::Mouse::Get().GetState().scrollWheelValue;
			char character = '\0';
			
			EntityPtr occupied = FindOccupiedRecursive(m_currentMenu, mousePos);
			Event evt = Event(occupied, mousePos, -scrollTicks, character, Keyboard::Keys::None);
			// hover out of all
			for (auto & hoverElement : m_hoverElements) {
				OnHoverOut(hoverElement, evt);
			}
			m_hoverElements.clear();
			OnHover(occupied,evt);
			
			switch (Game::MouseTracker.leftButton) {
			case ButtonState::PRESSED: OnMouseDown(occupied,evt);break;
			case ButtonState::HELD: OnDrag(m_activeElement,evt);break;
			case ButtonState::RELEASED: OnMouseUp(occupied,evt);break;
			}
			// scroll wheel
			
			DirectX::Mouse::Get().ResetScrollWheelValue();
			float deltaScrollPercent = (float)scrollTicks / (float)m_scrollTicks;
			if (deltaScrollPercent != 0.f) {
				occupied = FindFirstParent(occupied, [](shared_ptr<Style> style) {
					return style->GetOverflowY() == OverflowType::Scroll;
				});
				if (occupied) {
					OnScroll(occupied, evt);
					/*auto sprite = GetSprite(occupied);
					if (sprite) {
						sprite->ScrollPosition.y = Utility::Clamp(sprite->ScrollPosition.y + deltaScrollPercent, 0.f, 1.f);
						UpdateFlowRecursive(occupied, 1);
					}*/
				}
			}

			//for (int i = m.size() - 1; i >= 0; i--) {
			//	shared_ptr<Entity> entity;
			//	shared_ptr<Sprite> sprite = dynamic_pointer_cast<Sprite>(m_drawQueue[i]);
			//	if (sprite && GuiEM.Find(sprite->ID, entity)) {
			//		// Note: this assumes styles are not dynamically added
			//		if (sprite->Rect.Contains(mousePos)) {
			//			
			//			break;
			//		}
			//	}
			//}
		}
	}
	if (m_handMenu) {
		if (Game::MouseState.positionMode == Mouse::MODE_ABSOLUTE) {
			Vector2 mousePos(Game::MouseState.x, Game::MouseState.y);
			auto sprite = GetSprite(m_handMenu);
			sprite->Rect.x = mousePos.x;
			sprite->Rect.y = mousePos.y;
			UpdateFlowRecursive(m_handMenu, 1);
		}
	}
}

void GuiSystem::UpdateUI(int outputWidth, int outputHeight)
{
	m_outputRect = Rectangle(0, 0, outputWidth, outputHeight);
	UpdateUI();
}

void GuiSystem::UpdateUI()
{
	if (m_currentMenu) {
		// Set the menu to full-screen
		GetSprite(m_currentMenu)->Rect = m_outputRect;
		// Update the flow
		UpdateFlowRecursive(m_currentMenu, 1);
	}
}

void GuiSystem::OpenMenu(string name)
{
	OpenMenu(GetMenu(name));
	m_currentMenuName = name;
}

EntityPtr GuiSystem::GetCurrentMenu()
{
	return m_currentMenu;
}

EntityPtr GuiSystem::GetHandMenu()
{
	return m_handMenu;
}

void GuiSystem::SetHandMenu(EntityPtr element)
{
	m_handMenu = element;
}

void GuiSystem::CloseMenu()
{
	m_currentMenu = nullptr;
	m_handMenu = nullptr;
	m_currentMenuName = "";
}

GuiEntityManager & GuiSystem::GetEM()
{
	return GuiEM;
}

void GuiSystem::DisplayException(string message)
{
	OpenMenu(GuiEM.NewPanel([] {
		Style * style = new Style();
		style->Justify = "center";
		return style;
	}(), vector<EntityPtr>{
		// Header
		[=] {
			auto panel = GuiEM.NewPanel([] {
				Style * style = new Style();
				style->Background = "rgb(0.5,0.1,0.1)";
				style->FontSize = "48px";
				style->TextAlign = "center";
				style->Height = "20%";
				return style;
			}());
			GuiEM.AddText(panel, message);
			return panel;
		}(),
			MainMenuBtn(48)
	}));
}

void GuiSystem::SetTextByID(string id, string text)
{
	GuiEM.AddText(GetElementByID(id), text);
}

void GuiSystem::ShowHint(string hint)
{
	SetTextByID("HUD", hint);
}

shared_ptr<Style> GuiSystem::GetStyle(string id)
{
	return GetStyle(GetElementByID(id));
}

void GuiSystem::BindHandlers()
{
	IEventManager::RegisterHandler(GUI_ShowHint, std::function<void(string)>([=](string text) {
		ShowHint(text);
	}));
	IEventManager::RegisterHandler(GUI_HideHint, std::function<void(void)>([=]() {
		ShowHint("");
	}));

	IEventManager::RegisterHandler(GUI_OpenMenu, std::function<void(string)>([this](string menu) {
		OpenMenu(menu);
	}));
	IEventManager::RegisterHandler(GUI_CloseMenu, std::function<void()>([this] {
		CloseMenu();
	}));
}

void GuiSystem::CharTyped(char ch)
{
	OnKeydown(m_activeElement, Event(m_activeElement,Vector2::Zero, 0, ch,Keyboard::Keys::None));
}

void GuiSystem::Backspace()
{
	OnKeydown(m_activeElement, Event(m_activeElement, Vector2::Zero, 0, '\0', Keyboard::Keys::Back));
}

EntityPtr GuiSystem::ImportMarkup(string path, GUI::GuiHandler * handler)
{
	return CreateElementFromXml(AssetManager::Get()->GetXml(path),handler);
}

EntityPtr GuiSystem::CreateElementFromXml(shared_ptr<XmlParser> xml, GUI::GuiHandler * handler)
{
	auto attributes = xml->GetAttributes();
	//----------------------------------------------------------------
	// Default Style
	Style * style;
	
	if (attributes.find("style") != attributes.end()) {
		style = ParseStyle("Default", attributes["style"]);
	}
	else {
		style = new Style();
	}
	//----------------------------------------------------------------
	// ID
	string id = attributes.find("id") != attributes.end() ? attributes["id"] : "";
	

	//----------------------------------------------------------------
	// Children
	vector<EntityPtr> children;
	// recursively create children
	for (auto & childXml : xml->GetChildren()) {
		children.push_back(CreateElementFromXml(childXml));
	}

	// construct the element
	EntityPtr element = GuiEM.NewPanel(style, children, id);
	//----------------------------------------------------------------
	// Event bubbling
	if (attributes.find("event-bubbling") != attributes.end()) {
		if (attributes["event-bubbling"] == "true") {
			element->GetComponent<GUI::Panel>("Panel")->EventBubbling = true;
		}
	}
	//----------------------------------------------------------------
	// Text
	if (attributes.find("text") != attributes.end()) {
		element->AddComponent(new Text(attributes["text"]));
	}
	//----------------------------------------------------------------
	// Other styles
	if (attributes.find("hover-style") != attributes.end()) {
		element->AddComponent(ParseStyle("Hover", attributes["hover-style"]));
	}
	if (attributes.find("active-style") != attributes.end()) {
		element->AddComponent(ParseStyle("Active", attributes["active-style"]));
	}
	//----------------------------------------------------------------
	// Event handlers
	if (handler) {
		for (auto & attribute : attributes) {
			auto eventHandler = std::shared_ptr<Components::Component>(new GUI::EventHandler(attribute.first, [=](Event evt) {
				handler->InvokeEvent(attribute.second, evt);
			}));
			auto mask = GuiEM.ComponentMask(eventHandler->GetName());
			if (mask) {
				element->AddComponent(eventHandler);
			}
		}
	}

	return element;
}

Style * GuiSystem::ParseStyle(string selector, string cssString)
{
	Style * style = new Style(selector);
	CssParser css(cssString);
	for (auto & pair : css.GetStyles()) {
		if (pair.first == "background") {
			style->Background = pair.second;
		}
		else if (pair.first == "font-color") {
			style->FontColor = pair.second;
		}
		else if (pair.first == "foreground") {
			style->Foreground = pair.second;
		}
		else if (pair.first == "width") {
			style->Width = pair.second;
		}
		else if (pair.first == "width-2") {
			style->Width2 = pair.second;
		}
		else if (pair.first == "height") {
			style->Height = pair.second;
		}
		else if (pair.first == "height-2") {
			style->Height2 = pair.second;
		}
		else if (pair.first == "flow-direction") {
			style->FlowDirection = pair.second;
		}
		else if (pair.first == "overflow-x") {
			style->OverflowX = pair.second;
		}
		else if (pair.first == "overflow-y") {
			style->OverflowY = pair.second;
		}
		else if (pair.first == "justify") {
			style->Justify = pair.second;
		}
		else if (pair.first == "align-items") {
			style->AlignItems = pair.second;
		}
		else if (pair.first == "font") {
			style->Font = pair.second;
		}
		else if (pair.first == "font-size") {
			style->FontSize = pair.second;
		}
		else if (pair.first == "text-align") {
			style->TextAlign = pair.second;
		}
		else if (pair.first == "vertical-text-align") {
			style->VerticalTextAlign = pair.second;
		}
		else if (pair.first == "visibility") {
			style->Visibility = pair.second;
		}
		else if (pair.first == "wrap-items") {
			style->WrapItems = pair.second == "true";
		}
	}
	return style;
}

void GuiSystem::AddMenu(string name, EntityPtr menu)
{
	// Add scrollbars as necessary
	AddScrollbarsRecursive(menu);
	// Cache the menu
	m_menus.insert(std::make_pair(name, menu));
}

void GuiSystem::AddDynamicMenu(string name, std::function<EntityPtr(void)> constructor)
{
	m_dynamicMenus.insert(std::make_pair(name, constructor));
}

EntityPtr GuiSystem::GetMenu(string name)
{
	if (m_menus.find(name) != m_menus.end()) {
		return m_menus[name];
	}
	else if (m_dynamicMenus.find(name) != m_dynamicMenus.end()) {
		auto menu = m_dynamicMenus[name]();
		// Add scrollbars as necessary
		AddScrollbarsRecursive(menu);
		return menu;
	}
	else {
		auto system = SM->GetSystem<GuiHandler>(name);
		if (system) {
			return system->GetElement();
		}
	}
	
	return nullptr;
}

void GuiSystem::OpenMenu(EntityPtr menu)
{
	if (menu) {
		m_currentMenu = menu;
		m_handMenu = nullptr;
		UpdateUI(m_outputRect.width, m_outputRect.height);
	}
}

bool GuiSystem::IsMenuOpen(string name)
{
	return m_currentMenuName == name;
}

//EntityPtr GuiSystem::CreateInventory()
//{
//	
//	EntityPtr inv = ImportMarkup("UI/inv.xml");
//	EntityPtr header = FindElementByIdRecursive(inv, "header");
//	EntityPtr body = FindElementByIdRecursive(inv, "body");
//
//	vector<EntityPtr> tabs;
//	EntityPtr tabTemplate = ImportMarkup("UI/inv_tab.xml");
//
//	
//	static string currentCategory = "All";
//	auto categories = SM->GetSystem<world::ItemSystem>("Item")->GetItemCatagories();
//	categories.insert("craft");
//	for (string category : categories) {
//		tabs.push_back([=] {
//			EntityPtr tab = GuiEM.Copy(tabTemplate);
//			GuiEM.AddText(tab, category);
//			GuiEM.AddEventHandler(tab,new GUI::EventHandler("Click", [=](GUI::Event evt) {
//				currentCategory = category;
//				SelectInventoryTab(body, category);
//			}));
//			return tab;
//		}());
//	}
//	
//	// Initialize the current tab
//	SelectInventoryTab(body, currentCategory);
//	// Add the tabs to the header
//	GuiEM.AddChildren(header, tabs);
//	return inv;
//}
//
//EntityPtr GuiSystem::CreateInventoryGrid(string gridTemplate,vector<world::InventoryItem> items)
//{
//	auto invGrid = ImportMarkup(gridTemplate);
//	auto cellTemplate = ImportMarkup("UI/inv_item.xml");
//	vector<EntityPtr> rows;
//	for (auto & item : items) {
//		auto itemType = SM->GetSystem<world::ItemSystem>("Item")->TypeOf(item)->GetComponent<Item>("Item");
//		// Add a grid cell
//		auto cell = GuiEM.Copy(cellTemplate);
//		GuiEM.AddEventHandler(cell, new GUI::EventHandler("Click", [=](Event evt) {
//			m_handMenu = GuiEM.Copy(cell);
//		}));
//		// label
//		auto label = FindElementByIdRecursive(cell, "label");
//		if (label) GuiEM.AddText(label, itemType->Name);
//
//		GuiEM.AddText(cell, to_string(item.Quantity));
//		GetStyle(cell)->Background = itemType->Sprite;
//
//		GuiEM.AddChild(invGrid, cell);
//	}
//	return invGrid;
//}

//void GuiSystem::SelectInventoryTab(EntityPtr gridContainer, string category)
//{
//	auto & itemSystem = SM->GetSystem<world::ItemSystem>("Item");
//	DeleteChildren(gridContainer);
//	EntityPtr openContainer = itemSystem->GetOpenContainer();
//	if (openContainer && openContainer != itemSystem->GetPlayer()) {
//		EntityPtr playerGrid = CreateInventoryGrid(
//			"UI/half_inv_grid.xml",
//			itemSystem->ItemsInCategory(
//				itemSystem->GetPlayerInventory(),
//				category
//			)
//		);
//		GuiEM.AddChild(gridContainer, playerGrid);
//		GuiEM.AddChild(gridContainer, ImportMarkup("UI/inv_divider.xml"));
//		EntityPtr containerGrid = CreateInventoryGrid(
//			"UI/half_inv_grid.xml",
//			itemSystem->ItemsInCategory(
//				itemSystem->GetInventoryOf(openContainer),
//				category
//			)
//		);
//		GuiEM.AddChild(gridContainer, containerGrid);
//	}
//	else {
//		EntityPtr grid = CreateInventoryGrid(
//			"UI/inv_grid.xml",
//			SM->GetSystem<ItemSystem>("Item")->ItemsInCategory(
//				SM->GetSystem<ItemSystem>("Item")->GetPlayerInventory(),
//				category
//			)
//		);
//		GuiEM.AddChild(gridContainer, grid);
//	}
//	
//	UpdateFlowRecursive(gridContainer, 1);
//}

void GuiSystem::AddSpriteSheet(string path, map<string, Rectangle> mapping)
{
	m_spriteSheets.insert(std::pair<string, map<string, Rectangle>>(path, mapping));
}

Rectangle GuiSystem::GetSpriteRect(string filePath, string spriteName)
{
	if (m_spriteSheets.find(filePath) != m_spriteSheets.end()) {
		auto spriteMapping = m_spriteSheets[filePath];
		if (spriteMapping.find(spriteName) != spriteMapping.end()) {
			return spriteMapping[spriteName];
		}
	}
	return Rectangle(0, 0, 0, 0);
}

void GuiSystem::UpdateFlowRecursive(EntityPtr entity,int zIndex)
{
	if (entity->HasComponents(GuiEM.ComponentMask(vector<string>{"Style_Default","Sprite"}))) {
		//----------------------------------------------------------------
		// Components
		shared_ptr<Style> style = entity->GetComponent<Style>("Style_Default");
		shared_ptr<Sprite> sprite = entity->GetComponent<Sprite>("Sprite");
		shared_ptr<Text> text = entity->GetComponent<Text>("Text");
		if (sprite && style) {
			
			//----------------------------------------------------------------
			// Background Sprite
			UpdateSprite(entity, style, sprite, zIndex);
			//----------------------------------------------------------------
			// Text
			if (text) {
				UpdateText(text, sprite, style);
			}
		}
		
		PositionChildren(entity);
		//----------------------------------------------------------------
		// Recurse over children
		if (entity->HasComponents(GuiEM.ComponentMask("Children"))) {
			// Update the flow for each child
			for (auto & childID : entity->GetComponent<Children>("Children")->Entities) {
				EntityPtr child;
				if (GuiEM.Find(childID, child)) {
					UpdateFlowRecursive(child,zIndex+1);
				}
			}
		}
	}
}

void GuiSystem::OnHover(EntityPtr entity, Event evt)
{
	if (m_activeElement != entity) {
		if (entity->HasComponents(GuiEM.ComponentMask("Style_Hover"))) {
			shared_ptr<Style> hoverStyle = entity->GetComponent<Style>("Style_Hover");
			shared_ptr<Sprite> sprite = GetSprite(entity);
			UpdateSprite(entity, hoverStyle, sprite, sprite->Zindex);
		}
		m_hoverElements.insert(entity);
	}
	auto panel = entity->GetComponent<GUI::Panel>("Panel");
	EntityPtr parent;
	if (panel->EventBubbling && panel->Parent && GuiEM.Find(panel->Parent, parent)) {
		OnHover(parent, evt);
	}
}

void GuiSystem::OnHoverOut(EntityPtr entity, Event evt)
{
	if (entity != m_activeElement) {
		OnDefault(entity);
	}
}

void GuiSystem::OnMouseDown(EntityPtr entity, Event evt)
{
	if (m_activeElement) OnDefault(m_activeElement);
	m_activeElement = entity;
	if (entity->HasComponents(GuiEM.ComponentMask("Style_Active"))) {
		shared_ptr<Style> activeStyle = entity->GetComponent<Style>("Style_Active");
		shared_ptr<Sprite> sprite = GetSprite(entity);
		UpdateSprite(entity, activeStyle, sprite, sprite->Zindex);
	}
}

void GuiSystem::OnMouseUp(EntityPtr entity, Event evt)
{
	shared_ptr<GUI::EventHandler> handler = entity->GetComponent<GUI::EventHandler>("EventHandler_MouseUp");
	if (handler) {
		evt.Sender = entity;
		handler->Callback(evt);
	}
	else {
		if (m_activeElement == entity) {
			OnClick(entity, evt);
		}
		if (m_activeElement) OnDefault(m_activeElement);
		m_activeElement = nullptr;
		OnHover(entity, evt);
	}
}

void GuiSystem::OnClick(EntityPtr entity, Event evt)
{
	if (entity->HasComponents(GuiEM.ComponentMask("EventHandler_Click"))) {
		shared_ptr<GUI::EventHandler> clickHandler = entity->GetComponent<GUI::EventHandler>("EventHandler_Click");
		evt.Sender = entity;
		if (clickHandler) clickHandler->Callback(evt);
	}
	shared_ptr<GUI::Panel> panel = entity->GetComponent<GUI::Panel>("Panel");
	EntityPtr parent;
	if (panel && panel->EventBubbling && GuiEM.Find(panel->Parent, parent)) {
		OnClick(parent, evt);
	}
	OnHover(entity,evt);
}

void GuiSystem::OnDrag(EntityPtr entity, Event evt)
{
	if (entity->HasComponents(GuiEM.ComponentMask("EventHandler_Drag"))) {
		evt.Sender = entity;
		shared_ptr<GUI::EventHandler> handler = entity->GetComponent<GUI::EventHandler>("EventHandler_Drag");
		if (handler) handler->Callback(evt);
	}
}

void GuiSystem::OnScroll(EntityPtr entity, Event evt)
{
	if (entity->HasComponents(GuiEM.ComponentMask("EventHandler_Scroll"))) {
		evt.Sender = entity;
		shared_ptr<GUI::EventHandler> handler = entity->GetComponent<GUI::EventHandler>("EventHandler_Scroll");
		if (handler) handler->Callback(evt);
	}
}

void GuiSystem::OnKeydown(EntityPtr entity, Event evt)
{
	if (entity) {
		evt.Sender = entity;
		shared_ptr<GUI::EventHandler> handler = entity->GetComponent<GUI::EventHandler>("EventHandler_Keydown");
		if (handler) handler->Callback(evt);
	}
}

void GuiSystem::OnDefault(EntityPtr entity)
{
	if (entity->HasComponents(GuiEM.ComponentMask(vector<string>{"Style_Default", "Sprite"}))) {
		shared_ptr<Style> defaultStyle = entity->GetComponent<Style>("Style_Default");
		shared_ptr<Sprite> sprite = GetSprite(entity);
		UpdateSprite(entity, defaultStyle, sprite, sprite->Zindex);
	}
}

EntityPtr GuiSystem::FindOccupiedRecursive(EntityPtr entity, Vector2 mousePos)
{
	EntityPtr occupied = entity;
	shared_ptr<Sprite> sprite = GetSprite(entity);
	if (entity->HasComponents(GuiEM.ComponentMask("Children"))) {
		auto children = entity->GetComponent<Children>("Children");
		for (auto & childID : children->Entities) {
			EntityPtr child;
			if (GuiEM.Find(childID, child)) {
				auto childSprite = GetSprite(child);
				if (childSprite && childSprite->Rect.Contains(mousePos)) {
					occupied = FindOccupiedRecursive(child,mousePos);
				}
			}
		}
	}
	return occupied;
}

EntityPtr GuiSystem::FindFirstParent(EntityPtr child, std::function<bool(shared_ptr<Style>)>&& validation)
{
	EntityPtr parent;
	shared_ptr<Panel> childPanel = child->GetComponent<Panel>("Panel");
	if (childPanel && childPanel->Parent != child->ID() && GuiEM.Find(childPanel->Parent, parent)) {
		shared_ptr<Style> parentStyle = parent->GetComponent<Style>("Style_Default");
		if (validation(parentStyle)) return parent;
		return FindFirstParent(parent, std::move(validation));
	}
	else {
		return nullptr;
	}
}

void GuiSystem::UpdateSprite(EntityPtr entity, shared_ptr<Style> style, shared_ptr<Sprite> sprite, int zIndex)
{
	sprite->BackgroundColor = Colors::Transparent;
	sprite->BackgroundImage = "";
	sprite->UseSourceRect = false;
	sprite->Visible = style->GetVisibility() == VisibilityType::Visible;
	switch (style->GetBackgroundType()) {
	case ContentType::SpriteSheet: 
		sprite->BackgroundImage = style->GetBackgroundImage();
		sprite->SourceRect = GetSpriteRect(sprite->BackgroundImage, style->GetBackgroundSprite());
		sprite->UseSourceRect = true;
		break;
	case ContentType::Image: 
		sprite->BackgroundImage = style->GetBackgroundImage();
		break;
	case ContentType::Color: 
		sprite->BackgroundColor = style->GetBackgroundColor();
		break;
	
	}
}

void GuiSystem::UpdateText(shared_ptr<Text> text, shared_ptr<Sprite> sprite, shared_ptr<Style> style)
{
	text->Font = style->Font;
	text->Position.x = sprite->Rect.x;
	text->Position.y = sprite->Rect.y;
	text->Color = style->GetFontColor();

	float fontSize = 0.f;
	switch (style->GetFontSize(fontSize)) {
	case DimensionType::Percent: text->FontSize = fontSize * sprite->Rect.height; break;
	case DimensionType::Pixel: text->FontSize = (int)fontSize; break;
	}

	// align text
	auto spriteFont = AssetManager::Get()->GetFont(text->Font,text->FontSize);

	Vector2 boundingRect = spriteFont->MeasureString(ansi2unicode(text->String).c_str());
	// horizontal
	switch (style->GetTextAlign()) {
	case AlignmentType::Center: text->Position.x += (int)(sprite->Rect.width / 2 - boundingRect.x / 2); break;
	case AlignmentType::End:text->Position.x += (int)(sprite->Rect.width - boundingRect.x); break;
	}
	// vertical
	switch (style->GetVerticalTextAlign()) {
	case AlignmentType::Center: text->Position.y += (int)(sprite->Rect.height / 2 - boundingRect.y / 2); break;
	case AlignmentType::End: text->Position.y += (int)(sprite->Rect.height - boundingRect.y); break;
	}
}

void GuiSystem::SetStyle(EntityPtr entity, Style style)
{
	auto current = entity->GetComponent<Style>(style.DelegateName);
	*current = style;
}

void GuiSystem::PositionChildren(EntityPtr parent)
{
	
	
	if (parent->HasComponents(GuiEM.ComponentMask("Children"))) {
		shared_ptr<Sprite> parentSprite = parent->GetComponent<Sprite>("Sprite");
		shared_ptr<Style> parentStyle = parent->GetComponent<Style>("Style_Default");
		shared_ptr<Children> children = parent->GetComponent<Children>("Children");
		
		Rectangle parentRect = parentSprite->Rect;


		FlowType flow = parentStyle->GetFlowDirection();
		AlignmentType alignment = parentStyle->GetAlignItems();
		AlignmentType justify = parentStyle->GetJustify();

		vector<shared_ptr<Sprite>> allChildSprites;
		//----------------------------------------------------------------
		// Calculate row rectangles
		vector<vector<Rectangle>> rowChildren;
		vector<Rectangle> rows;

		vector<Rectangle> childRects;

		int totalChildPrimary = 0;
		int totalChildSecondary = 0;
		int i = 0;
		const int lastChildIndex = children->Entities.size() - 1;
		for (auto & childID : children->Entities) {
			EntityPtr child;
			if (GuiEM.Find(childID, child)) {
				auto childSprite = child->GetComponent<Sprite>("Sprite");
				allChildSprites.push_back(childSprite);
				if (childSprite) {
					Rectangle childRect = CalculateChildRect(parentRect, child->GetComponent<Style>("Style_Default"));
					int primary = GetPrimaryDimension(flow, childRect);
					if (i == lastChildIndex) {
						childRects.push_back(childRect);
						totalChildPrimary += primary;
						totalChildSecondary = std::max(totalChildSecondary, GetSecondaryDimension(flow, childRect));
					}
					if (i == lastChildIndex || parentStyle->WrapItems && totalChildPrimary + primary >= GetPrimaryDimension(flow, parentRect)) {
						// A new row
						Rectangle rowRect;
						SetDimension(flow, rowRect, totalChildPrimary, totalChildSecondary);
						rows.push_back(rowRect);
						rowChildren.push_back(childRects);
						// reset variables
						childRects.clear();
						totalChildPrimary = 0;
						totalChildSecondary = 0;
					}
					if (i != lastChildIndex) {
						childRects.push_back(childRect);
						totalChildPrimary += primary;
						totalChildSecondary = std::max(totalChildSecondary, GetSecondaryDimension(flow, childRect));
					}
					
				}
			}
			i++;
		}
		//----------------------------------------------------------------
		// Position rows
		PositionRects(parentRect, rows, flow == FlowType::Column ? FlowType::Row : FlowType::Column, alignment, justify);
		//----------------------------------------------------------------
		// Position row children
		vector<Rectangle> allChildRects;
		int childIndex = 0;
		for (int rowIndex = 0; rowIndex < rows.size(); rowIndex++) {
			PositionRects(rows[rowIndex], rowChildren[rowIndex], flow, justify, alignment);
			for (auto & childRect : rowChildren[rowIndex]) {
				allChildSprites[childIndex]->Rect = childRect;
				childIndex++;
			}
			totalChildPrimary = std::max(totalChildPrimary, GetPrimaryPosition(flow,rows[rowIndex]) + GetPrimaryDimension(flow, rows[rowIndex]));
			totalChildSecondary = std::max(totalChildSecondary, GetSecondaryPosition(flow, rows[rowIndex]) + GetSecondaryDimension(flow, rows[rowIndex]));
		}
		////----------------------------------------------------------------
		//// Get total childen length
		//int totalChildPrimary = 0;
		//int totalChildSecondary = 0;
		//for (auto & childID : children->Entities) {
		//	EntityPtr child;
		//	if (GuiEM.Find(childID, child)) {
		//		auto childSprite = child->GetComponent<Sprite>("Sprite");
		//		if (childSprite) {
		//			childSprites.push_back(childSprite);
		//			childSprite->Rect = CalculateChildRect(parentRect, child->GetComponent<Style>("Style_Default"));
		//			totalChildPrimary += GetPrimaryDimension(flow, childSprite->Rect);
		//			int secondary = GetSecondaryDimension(flow, childSprite->Rect);
		//			if (secondary > totalChildSecondary) {
		//				totalChildSecondary = secondary;
		//			}
		//		}
		//	}
		//}
		//int primary = std::max(GetPrimaryDimension(flow, parentRect), totalChildPrimary);
		//int primaryBisector = primary / 2;
		//int secondary = std::max(GetSecondaryDimension(flow, parentRect),totalChildSecondary);
		//int secondaryBisector = secondary / 2;

		//----------------------------------------------------------------
		// Get total dimensions of the content
		//totalChildPrimary = maxChildPrimary;
		//totalChildSecondary = rowOffset;
		//----------------------------------------------------------------
		// Scroll offsets
		Vector2 contentSize = GetVector(flow, totalChildPrimary, totalChildSecondary);
		Vector2 parentSize = GetVector(flow, GetPrimaryDimension(flow, parentRect), GetSecondaryDimension(flow, parentRect));
		Vector2 parentPos = GetVector(flow, GetPrimaryPosition(flow, parentRect), GetSecondaryPosition(flow, parentRect));
		Vector2 contentPos = parentPos - (contentSize - parentSize) * parentSprite->ScrollPosition;
		if (contentSize.y <= parentSize.y) {
			contentPos.y = parentPos.y;
		}
		if (contentSize.x <= parentSize.x) {
			contentPos.x = parentPos.x;
		}
		// Create clipping rect
		parentSprite->ClippingRects.clear();
		if (parentStyle->GetOverflowY() == OverflowType::Scroll) {
			AddRectIfValid(parentRect, parentSprite->ClippingRects);
		}
		////----------------------------------------------------------------
		//// Calculate primary axis offsets
		//int primaryOffset = GetPrimaryPosition(flow, parentRect);
		//switch (parentStyle->GetJustify()) {
		//case AlignmentType::Center: primaryOffset += (primaryBisector - totalChildPrimary / 2); break;
		//case AlignmentType::End: primaryOffset += (primary - totalChildPrimary); break;
		//}
		//int secondaryOffset = GetSecondaryPosition(flow, parentRect);
		//----------------------------------------------------------------
		// Apply offsets
		for (auto & childSprite : allChildSprites) {
			//Rectangle & childRect = childSprite->Rect;
			//
			//// secondary
			//int childSecondaryOffset = secondaryOffset;
			//switch (parentStyle->GetAlignItems()) {
			//case AlignmentType::Center: childSecondaryOffset += (secondaryBisector - GetSecondaryDimension(flow, childRect) / 2);break;
			//case AlignmentType::End: childSecondaryOffset += (secondary - GetSecondaryDimension(flow, childRect));break;
			//}
			// apply scroll offsets
			childSprite->Rect.x += contentPos.x - parentRect.x;
			childSprite->Rect.y += contentPos.y - parentRect.y;
			// move the primary offset past this child
			//primaryOffset += GetPrimaryDimension(flow, childRect);
		}
	}
}

void GuiSystem::PositionRects(Rectangle & parent, vector<Rectangle> & children, FlowType flow, AlignmentType & justify, AlignmentType & alignment)
{
	//----------------------------------------------------------------
	// Get total childen length
	int totalChildPrimary = 0;
	int totalChildSecondary = 0;
	for (auto & child : children) {
		totalChildPrimary += GetPrimaryDimension(flow, child);
		totalChildSecondary = std::max(totalChildSecondary,GetSecondaryDimension(flow, child));
	}
	int primary = std::max(GetPrimaryDimension(flow, parent), totalChildPrimary);
	int primaryBisector = primary / 2;
	int secondary = std::max(GetSecondaryDimension(flow, parent), totalChildSecondary);
	int secondaryBisector = secondary / 2;

	//----------------------------------------------------------------
	// Calculate primary axis offsets
	int primaryOffset = GetPrimaryPosition(flow, parent);
	switch (justify) {
	case AlignmentType::Center: primaryOffset += (primaryBisector - totalChildPrimary / 2); break;
	case AlignmentType::End: primaryOffset += (primary - totalChildPrimary); break;
	}
	int secondaryOffset = GetSecondaryPosition(flow, parent);
	//----------------------------------------------------------------
	// Apply offsets
	for (auto & child : children) {

		// secondary
		int childSecondaryOffset = secondaryOffset;
		switch (alignment) {
		case AlignmentType::Center: childSecondaryOffset += (secondaryBisector - GetSecondaryDimension(flow, child) / 2);break;
		case AlignmentType::End: childSecondaryOffset += (secondary - GetSecondaryDimension(flow, child));break;
		}
		SetPosition(flow, child, primaryOffset, childSecondaryOffset);
		// move the primary offset past this child
		primaryOffset += GetPrimaryDimension(flow, child);
	}
}

Rectangle GuiSystem::CalculateChildRect(Rectangle parentRect, shared_ptr<Style> childStyle)
{
	Rectangle childRect(parentRect);
	float width;
	switch (childStyle->GetWidth(width)) {
	case DimensionType::Percent: childRect.width = (int)(width * parentRect.width); break;
	case DimensionType::Pixel: childRect.width = (int)width; break;
	}
	float height;
	switch (childStyle->GetHeight(height)) {
	case DimensionType::Percent: childRect.height = (int)(height * parentRect.height); break;
	case DimensionType::Pixel: childRect.height = (int)height; break;
	}
	// Add second width and height
	float width2;
	switch (childStyle->GetWidth2(width2)) {
	case DimensionType::Percent: childRect.width += (int)(width2 * parentRect.width); break;
	case DimensionType::Pixel: childRect.width += (int)width2; break;
	}
	float height2;
	switch (childStyle->GetHeight2(height2)) {
	case DimensionType::Percent: childRect.height += (int)(height2 * parentRect.height); break;
	case DimensionType::Pixel: childRect.height += (int)height2; break;
	}
	return childRect;
}

void GuiSystem::SetPosition(FlowType flow, Rectangle & rect, int primary, int secondary)
{
	switch (flow) {
	case FlowType::Column: rect.x = secondary; rect.y = primary;break;
	case FlowType::Row: rect.x = primary; rect.y = secondary;break;
	}
}

void GuiSystem::SetDimension(FlowType flow, Rectangle & rect, int primary, int secondary)
{
	switch (flow) {
	case FlowType::Column: rect.width = secondary; rect.height = primary;break;
	case FlowType::Row: rect.width = primary; rect.height = secondary;break;
	}
}

int GuiSystem::GetPrimaryDimension(FlowType flow, Rectangle rect)
{
	switch (flow) {
	case FlowType::Column: return rect.height;
	case FlowType::Row: return rect.width;
	}
	return 0;
}

int GuiSystem::GetSecondaryDimension(FlowType flow, Rectangle rect)
{
	switch (flow) {
	case FlowType::Column: return rect.width;
	case FlowType::Row: return rect.height;
	}
	return 0;
}

int GuiSystem::GetPrimaryPosition(FlowType flow, Rectangle rect)
{
	switch (flow) {
	case FlowType::Column: return rect.y;
	case FlowType::Row: return rect.x;
	}
	return 0;
}

int GuiSystem::GetSecondaryPosition(FlowType flow, Rectangle rect)
{
	switch (flow) {
	case FlowType::Column: return rect.x;
	case FlowType::Row: return rect.y;
	}
	return 0;
}

Vector2 GuiSystem::GetVector(FlowType flow, int primary, int secondary)
{
	switch (flow) {
	case FlowType::Column: return Vector2(secondary, primary);
	case FlowType::Row: return Vector2(primary, secondary);
	}
	return Vector2::Zero;
}

EntityPtr GuiSystem::NewVerticalScrollBar(EntityPtr target)
{
	auto targetSprite = target->GetComponent<Sprite>("Sprite");
	auto targetStyle = target->GetComponent<Style>("Style_Default");
	EntityPtr scrollbar = GuiEM.NewPanel([] {
		Style * style = new Style();
		style->Background = "rgb(0.25,0.25,0.25)";
		style->FlowDirection = "column";
		style->Height = "100%";
		style->Width = "32px";
		return style;
	}());
	auto scrollbarSprite = GetSprite(scrollbar);
	// Scrollbar handle ---------------
	 
	int handleHeight = 128;
	Style * style = new Style();
	style->Background = "widget.png:scroll_handle";
	style->Width = "32px";
	style->Height = to_string(handleHeight) + "px";

	EntityPtr handle = GuiEM.NewPanel(style);
	handle->AddComponent([] {
		Style * hover = new Style("Hover");
		hover->Background = "widget.png:scroll_handle_hover";
		return hover;
	}());
	handle->AddComponent([] {
		Style * active = new Style("Active");
		active->Background = "widget.png:scroll_handle_active";
		return active;
	}());
	auto dragHandler = new EventHandler("Drag", [=](Event evt) {
		auto sprite = GetSprite(handle);
		int top = scrollbarSprite->Rect.y;
		int bottom = scrollbarSprite->Rect.y + scrollbarSprite->Rect.height;
		int lastPos = sprite->Rect.y;
		int handlePos = std::min(bottom-handleHeight, std::max(top, (int)evt.MousePosition.y-handleHeight/2));
		if (handlePos != lastPos) {
			float scrollPos = scrollbarSprite->Rect.height > handleHeight ? float(handlePos - top) / float(bottom - handleHeight - top) : 0.f;
			sprite->Rect.y = handlePos;
			targetSprite->ScrollPosition.y = scrollPos;
			UpdateFlowRecursive(target, 1);
		}
	});
	GuiEM.AddEventHandler(handle, dragHandler);
	auto scrollHandler = new EventHandler("Scroll", [=](Event evt) {
		auto sprite = GetSprite(handle);
		int top = scrollbarSprite->Rect.y;
		int bottom = scrollbarSprite->Rect.y + scrollbarSprite->Rect.height;
		int lastPos = sprite->Rect.y;
		int handlePos = std::min(bottom - handleHeight, std::max(top, (int)sprite->Rect.y + evt.ScrollDelta));
		if (handlePos != lastPos) {
			float scrollPos = scrollbarSprite->Rect.height > handleHeight ? float(handlePos - top) / float(bottom - handleHeight - top) : 0.f;
			sprite->Rect.y = handlePos;
			targetSprite->ScrollPosition.y = scrollPos;
			UpdateFlowRecursive(target, 1);
		}
	});
	GuiEM.AddEventHandler(target, scrollHandler);

	GuiEM.AddChildren(scrollbar, vector<EntityPtr>{handle});
	//------------------------------------

	string width = targetStyle->Width;
	string height = targetStyle->Height;
	return GuiEM.NewPanel([=] {
		Style * style = new Style();
		style->FlowDirection = "row";
		style->Height = height;
		style->Width = width;
		return style;
	}(), vector<EntityPtr>{
		[=] {
			targetStyle->Height = "100%";
			targetStyle->Width = "100%";
			targetStyle->Width2 = "-32px";
			return target;
		}(),
		// Scrollbar (right)
		scrollbar
	});
}

EntityPtr GuiSystem::MainMenuBtn(int height)
{
	return GuiEM.NewButton("Main Menu", [=](Event evt) {
		Game::Get().CloseWorld();
		OpenMenu("main");
	}/*, [=] {
		Style * style = new Style();
		style->Background = "widget.png:button";
		Rectangle spriteRect = GetSpriteRect("widget.png", "button");
		style->Height = to_string(height) + "px";
		style->Width = to_string(int((float(height) / float(spriteRect.height)) * float(spriteRect.width))) + "px";
		style->FontColor = "rgb(1,1,1)";
		style->FontSize = "48px";
		return style;
	}()*/);
}

EntityPtr GuiSystem::NewTextBox(string id,string placeholder)
{
	auto textbox = GuiEM.NewPanel([] {
		Style * style = new Style();
		style->Height = "100px";
		style->FontColor = "rgb(1,1,1)";
		style->Background = "rgb(0.5,0.5,0.5)";
		return style;
	}(),vector<EntityPtr>(),id);
	Text * text = new Text(placeholder);
	textbox->AddComponent(text);
	textbox->AddComponent([=] {
		Style * style = new Style("Hover");
		style->Background = "rgb(0.75,0.75,0.75)";
		return style;
	}());
	textbox->AddComponent([=] {
		Style * style = new Style("Active");
		style->Background = "rgb(0,0,0.25)";
		return style;
	}());
	GuiEM.AddEventHandler(textbox, new EventHandler("Keydown", [=](Event evt) {
		switch (evt.Key) {
		case Keyboard::Keys::Back:
			text->String = text->String.substr(0,text->String.length() - 1);
			break;
		default:
			if (evt.Character != '\0') {
				text->String += evt.Character;
			}
		}
	}));
	GuiEM.AddEventHandler(textbox, new EventHandler("MouseUp", [=](Event evt) {
		m_activeElement = textbox;
	}));
	return textbox;
}

void GuiSystem::DeleteChildren(EntityPtr & parent)
{
	auto children = parent->GetComponent<Children>("Children");

	if (children) {
		for (auto & childID : children->Entities) {
			EntityPtr child;
			if (GuiEM.Find(childID, child)) {
				DeleteRecursive(child);
			}
		}
		children->Entities.clear();
	}
}

void GuiSystem::DeleteRecursive(EntityPtr & parent)
{
	auto children = parent->GetComponent<Children>("Children");
	
	if (children) {
		for (auto & childID : children->Entities) {
			EntityPtr child;
			if (GuiEM.Find(childID, child)) {
				DeleteRecursive(child);
			}
		}
	}
	GuiEM.DeleteEntity(parent);
}

void GuiSystem::ReplaceChildren(EntityPtr parent, vector<EntityPtr> children)
{
	DeleteChildren(parent);
	GuiEM.AddChildren(parent, children);
}

void GuiSystem::ReplaceChild(EntityPtr parent, EntityPtr child)
{
	DeleteChildren(parent);
	GuiEM.AddChild(parent, child);
}

shared_ptr<Sprite> GuiSystem::GetSprite(EntityPtr entity)
{
	return entity->GetComponent<Sprite>("Sprite");
}

shared_ptr<Style> GuiSystem::GetStyle(EntityPtr entity)
{
	return entity->GetComponent<Style>("Style_Default");
}

void GuiSystem::AddRectIfValid(Rectangle rect, vector<Rectangle>& rects)
{
	if (rect.width > 0 && rect.height > 0) rects.push_back(rect);
}

void GuiSystem::AddScrollbarsRecursive(EntityPtr & entity)
{
	auto children = entity->GetComponent<Children>("Children");
	if (children) {
		for (auto & childID : children->Entities) {
			EntityPtr child;
			if (GuiEM.Find(childID, child)) {
				AddScrollbarsRecursive(child);
				childID = child->ID();
			}
		}
	}
	auto style = entity->GetComponent<Style>("Style_Default");
	if (style && style->GetOverflowY() == OverflowType::Scroll) {
		unsigned int parent = entity->GetComponent<Panel>("Panel")->Parent;
		entity.swap(NewVerticalScrollBar(entity));
		entity->GetComponent<Panel>("Panel")->Parent = parent;
	}
}

EntityPtr GuiSystem::GetElementByID(string id)
{
	if (m_currentMenu) {
		return FindElementByIdRecursive(m_currentMenu, id);
	}
	return nullptr;
}

EntityPtr GuiSystem::FindElementByIdRecursive(EntityPtr entity, string id)
{
	auto panel = entity->GetComponent<Panel>("Panel");
	if (panel && panel->ElementID == id) return entity;
	// recurse over children
	auto children = entity->GetComponent<Children>("Children");
	EntityPtr result = nullptr;
	if (children) {
		for (auto & childID : children->Entities) {
			if (GuiEM.Find(childID, result)) {
				result = FindElementByIdRecursive(result,id);
				if (result) return result;
			}
		}
	}
	return nullptr;
}
