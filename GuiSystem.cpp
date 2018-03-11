#include "pch.h"
#include "GuiSystem.h"
#include "GuiStyle.h"
#include "GuiPanel.h"
#include "GuiChildren.h"
#include "GuiEventHandler.h"
#include "Game.h"
#include "GuiText.h"
#include "AssetManager.h"
#include "IEventManager.h"
#include "World.h"

using namespace GUI;
using namespace DirectX;
using ButtonState = DirectX::Mouse::ButtonStateTracker::ButtonState;

GuiSystem::GuiSystem(
	unsigned short updatePeriod
) : System::System(updatePeriod),
m_scrollTicks(1200)
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
				if (!Game::Get().LoadWorld()) {
					OpenMenu("load_world");
				}
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
				World * world = nullptr;
				if (Game::Get().TryGetWorld(world)) {
					world->ResumeGame();
				}
			}),
			GuiEM.NewButton("Main Menu", [=](Event evt) {
				Game::Get().CloseWorld();
				OpenMenu("main");
			})
		})
	}));
#pragma endregion
	//----------------------------------------------------------------
	// HUD
#pragma region HUD
	m_HUDhint = GuiEM.NewTextPanel("Hint", [] {
		Style * style = new Style();
		style->FontSize = "32px";
		style->TextAlign = "center";
		style->FontColor = "rgb(1,1,1)";
		style->VerticalTextAlign = "center";
		style->Height = "100%";
		return style;
	}());
	AddMenu("HUD", m_HUDhint);
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
							Game::Get().CloseWorld();
							// load up the new world
							Game::Get().LoadWorld(name->String);
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
}


GuiSystem::~GuiSystem()
{
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
			Event evt = Event(mousePos, -scrollTicks, character, Keyboard::Keys::None);
			EntityPtr occupied = FindOccupiedRecursive(m_currentMenu, mousePos);
			
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
}

void GuiSystem::UpdateUI(int outputWidth, int outputHeight)
{
	m_outputRect = Rectangle(0, 0, outputWidth, outputHeight);
	if (m_currentMenu) {
		// Set the menu to full-screen
		GetSprite(m_currentMenu)->Rect = Rectangle(0, 0, outputWidth, outputHeight);
		// Update the flow
		UpdateFlowRecursive(m_currentMenu,1);
	}
}

void GuiSystem::OpenMenu(string name)
{
	OpenMenu(GetMenu(name));
}

EntityPtr GuiSystem::GetCurrentMenu()
{
	return m_currentMenu;
}

void GuiSystem::CloseMenu()
{
	m_currentMenu = nullptr;
}

GuiEntityManager & GuiSystem::GetEM()
{
	return GuiEM;
}

void GuiSystem::DisplayException(std::exception e)
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
			GuiEM.AddText(panel, "An exception occured");
			return panel;
		}(),
			MainMenuBtn(48)
	}));
}

void GuiSystem::ShowHint(string hint)
{
	
	auto textComp = m_HUDhint->GetComponent<GUI::Text>("Text");
	textComp->String = hint;
	OpenMenu("HUD");
}

void GuiSystem::HideHint()
{
	CloseMenu();
}

void GuiSystem::BindHandlers()
{
	IEventManager::RegisterHandler(GUI_ShowHint, std::function<void(string)>([=](string text) {
		ShowHint(text);
	}));
	IEventManager::RegisterHandler(GUI_HideHint, std::function<void(void)>([=]() {
		HideHint();
	}));
}

void GuiSystem::CharTyped(char ch)
{
	OnKeydown(m_activeElement, Event(Vector2::Zero, 0, ch,Keyboard::Keys::None));
}

void GuiSystem::Backspace()
{
	OnKeydown(m_activeElement, Event(Vector2::Zero, 0, '\0', Keyboard::Keys::Back));
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
	return nullptr;
}

void GuiSystem::OpenMenu(EntityPtr menu)
{
	if (menu) {
		m_currentMenu = menu;
		UpdateUI(m_outputRect.width, m_outputRect.height);
	}
}

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
		if (m_hoverElement && m_hoverElement != entity) OnHoverOut(m_hoverElement,evt);
		if (entity->HasComponents(GuiEM.ComponentMask("Style_Hover"))) {
			shared_ptr<Style> hoverStyle = entity->GetComponent<Style>("Style_Hover");
			shared_ptr<Sprite> sprite = GetSprite(entity);
			UpdateSprite(entity, hoverStyle, sprite, sprite->Zindex);
		}
		m_hoverElement = entity;
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
		if (clickHandler) clickHandler->Callback(evt);
	}
	OnHover(entity,evt);
}

void GuiSystem::OnDrag(EntityPtr entity, Event evt)
{
	if (entity->HasComponents(GuiEM.ComponentMask("EventHandler_Drag"))) {
		shared_ptr<GUI::EventHandler> handler = entity->GetComponent<GUI::EventHandler>("EventHandler_Drag");
		if (handler) handler->Callback(evt);
	}
}

void GuiSystem::OnScroll(EntityPtr entity, Event evt)
{
	if (entity->HasComponents(GuiEM.ComponentMask("EventHandler_Scroll"))) {
		shared_ptr<GUI::EventHandler> handler = entity->GetComponent<GUI::EventHandler>("EventHandler_Scroll");
		if (handler) handler->Callback(evt);
	}
}

void GuiSystem::OnKeydown(EntityPtr entity, Event evt)
{
	if (entity) {
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
				if (childSprite->Rect.Contains(mousePos)) {
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
		vector<shared_ptr<Sprite>> childSprites;
		
		//----------------------------------------------------------------
		// Get total childen length
		int totalChildPrimary = 0;
		int totalChildSecondary = 0;
		for (auto & childID : children->Entities) {
			EntityPtr child;
			if (GuiEM.Find(childID, child)) {
				auto childSprite = child->GetComponent<Sprite>("Sprite");
				if (childSprite) {
					childSprites.push_back(childSprite);
					childSprite->Rect = CalculateChildRect(parentRect, child->GetComponent<Style>("Style_Default"));
					totalChildPrimary += GetPrimaryDimension(flow, childSprite->Rect);
					int secondary = GetSecondaryDimension(flow, childSprite->Rect);
					if (secondary > totalChildSecondary) {
						totalChildSecondary = secondary;
					}
				}
			}
		}
		int primary = std::max(GetPrimaryDimension(flow, parentRect), totalChildPrimary);
		int primaryBisector = primary / 2;
		int secondary = std::max(GetSecondaryDimension(flow, parentRect),totalChildSecondary);
		int secondaryBisector = secondary / 2;

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
		// Create clipping rects
		/*float topDiff = parentRect.y - contentPos.y;
		float bottomDiff = (contentPos.y + contentSize.y) - (parentRect.y + parentRect.height);
		float leftDiff = parentRect.x - contentPos.x;
		float rightDiff = (contentSize.x + contentPos.x) - (parentRect.x + parentRect.width);*/

		/*Rectangle clipTop = Rectangle(contentPos.x, contentPos.y, contentSize.x, topDiff);
		Rectangle clipBottom = Rectangle(contentPos.x, parentRect.y + parentRect.height, contentSize.x, bottomDiff);
		Rectangle clipLeft = Rectangle(contentPos.x, contentPos.y + topDiff, leftDiff, parentRect.height);
		Rectangle clipRight = Rectangle(parentRect.x + parentRect.width, parentRect.y, rightDiff, parentRect.height);*/
		parentSprite->ClippingRects.clear();
		/*AddRectIfValid(clipTop, parentSprite->ClippingRects);
		AddRectIfValid(clipBottom, parentSprite->ClippingRects);
		AddRectIfValid(clipLeft, parentSprite->ClippingRects);
		AddRectIfValid(clipRight, parentSprite->ClippingRects);*/
		if (parentStyle->GetOverflowY() == OverflowType::Scroll) {
			AddRectIfValid(parentRect, parentSprite->ClippingRects);
		}
		//----------------------------------------------------------------
		// Calculate primary axis offsets
		int primaryOffset = GetPrimaryPosition(flow, parentRect);
		switch (parentStyle->GetJustify()) {
		case AlignmentType::Center: primaryOffset += (primaryBisector - totalChildPrimary / 2); break;
		case AlignmentType::End: primaryOffset += (primary - totalChildPrimary); break;
		}
		int secondaryOffset = GetSecondaryPosition(flow, parentRect);
		//----------------------------------------------------------------
		// Apply offsets
		for (auto & childSprite : childSprites) {
			Rectangle & childRect = childSprite->Rect;
			
			// secondary
			int childSecondaryOffset = secondaryOffset;
			switch (parentStyle->GetAlignItems()) {
			case AlignmentType::Center: childSecondaryOffset += (secondaryBisector - GetSecondaryDimension(flow, childRect) / 2);break;
			case AlignmentType::End: childSecondaryOffset += (secondary - GetSecondaryDimension(flow, childRect));break;
			}
			SetPosition(flow, childRect, primaryOffset, childSecondaryOffset);
			// apply scroll offsets
			childRect.x += contentPos.x - parentRect.x;
			childRect.y += contentPos.y - parentRect.y;
			// move the primary offset past this child
			primaryOffset += GetPrimaryDimension(flow, childRect);
		}
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

shared_ptr<Sprite> GuiSystem::GetSprite(EntityPtr entity)
{
	return entity->GetComponent<Sprite>("Sprite");
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
