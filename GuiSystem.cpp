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

using namespace GUI;
using namespace DirectX;
using ButtonState = DirectX::Mouse::ButtonStateTracker::ButtonState;

GuiSystem::GuiSystem(
	unsigned short updatePeriod
) : System::System(updatePeriod)
{
	//----------------------------------------------------------------
	// Initialize spritesheets
	AddSpriteSheet("widget.png", std::map<string, Rectangle>{
		{ "button", Rectangle(0, 0, 384, 128)},
		{ "button_hover",Rectangle(0,128,384,128) },
		{ "button_active",Rectangle(0,256,384,128) }
	});

	//----------------------------------------------------------------
	// Common styles
	// Generate the entire GUI

	//----------------------------------------------------------------
	// main

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
			GuiEM.NewButton("Continue", [=] {
				Game::Get().LoadWorld("test");
			}, [] {
				Style * style = new Style();
				style->Background = "widget.png:button";
				style->Height = "100px";
				style->FontColor = "rgb(1,1,1)";
				style->FontSize = "48px";
				return style;
			}()),
				GuiEM.NewButton("New World", [=] {
				OpenMenu("new_world");
			}, [] {
				Style * style = new Style();
				style->Background = "widget.png:button";
				style->Height = "100px";
				style->FontColor = "rgb(1,1,1)";
				style->FontSize = "48px";
				return style;
			}()),
				GuiEM.NewButton("Load World", [=] {
				OpenMenu("load_world");
			}, [] {
				Style * style = new Style();
				style->Background = "widget.png:button";
				style->Height = "100px";
				style->FontColor = "rgb(1,1,1)";
				style->FontSize = "48px";
				return style;
			}()),
				GuiEM.NewButton("Options", [=] {
				OpenMenu("options");
			}, [] {
				Style * style = new Style();
				style->Background = "widget.png:button";
				style->Height = "100px";
				style->FontColor = "rgb(1,1,1)";
				style->FontSize = "48px";
				return style;
			}())
		})
	}));

	//----------------------------------------------------------------
	// game_paused

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
			GuiEM.NewButton("Resume Game", [=] {
				Game::Get().ResumeGame();
			}, [] {
				Style * style = new Style();
				style->Background = "widget.png:button";
				style->Height = "100px";
				style->FontColor = "rgb(1,1,1)";
				style->FontSize = "48px";
				return style;
			}()),
				GuiEM.NewButton("Main Menu", [=] {
				Game::Get().CloseWorld();
				OpenMenu("main");
			}, [] {
				Style * style = new Style();
				style->Background = "widget.png:button";
				style->Height = "100px";
				style->FontColor = "rgb(1,1,1)";
				style->FontSize = "48px";
				return style;
			}())
		})
	}));
	//----------------------------------------------------------------
	// HUD
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
			GuiEM.NewButton("Controls", [=] {
				Game::Get().ResumeGame();
			}, [] {
				Style * style = new Style();
				style->Background = "widget.png:button";
				style->Height = "100px";
				style->FontColor = "rgb(1,1,1)";
				style->FontSize = "48px";
				return style;
			}()),
				GuiEM.NewButton("Main Menu", [=] {
				Game::Get().CloseWorld();
				OpenMenu("main");
			}, [] {
				Style * style = new Style();
				style->Background = "widget.png:button";
				style->Height = "100px";
				style->FontColor = "rgb(1,1,1)";
				style->FontSize = "48px";
				return style;
			}())
		})
	}));
	//----------------------------------------------------------------
	// New World
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
			// Side panel
			GuiEM.NewPanel([] {
			Style * style = new Style();
			style->Justify = "center";
			style->Height = "80%";
			style->Width = "20%";
			return style;
		}(), vector<EntityPtr>{
			GuiEM.NewButton("Generate!", [=] {
				Game::Get().GenerateWorld(23242,"test");
			}, [] {
				Style * style = new Style();
				style->Background = "widget.png:button";
				style->Height = "100px";
				style->FontColor = "rgb(0,0,0.5)";
				style->FontSize = "48px";
				return style;
			}()),
				GuiEM.NewButton("Main Menu", [=] {
				Game::Get().CloseWorld();
				OpenMenu("main");
			}, [] {
				Style * style = new Style();
				style->Background = "widget.png:button";
				style->Height = "100px";
				style->FontColor = "rgb(1,1,1)";
				style->FontSize = "48px";
				return style;
			}())
		})
	}));
	////----------------------------------------------------------------
	//// Controls
	//AddMenu("controls", GuiEM.NewPanel([] {
	//	Style * style = new Style();
	//	return style;
	//}(), vector<EntityPtr>{
	//	// Header
	//	[=] {
	//		auto panel = GuiEM.NewPanel([] {
	//			Style * style = new Style();
	//			style->Background = "rgba(1,1,1,0.5)";
	//			style->FontSize = "128px";
	//			style->TextAlign = "center";
	//			style->Height = "15%";
	//			return style;
	//		}());
	//		GuiEM.AddText(panel, "Controls");
	//		return panel;
	//	}(),
	//		// Side panel
	//		GuiEM.NewPanel([] {
	//		Style * style = new Style();
	//		style->Justify = "center";
	//		style->Height = "85%";
	//		style->Width = "20%";
	//		return style;
	//	}(), vector<EntityPtr>{}())
	//	})
	//}));

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
	if (Game::MouseState.positionMode == Mouse::MODE_ABSOLUTE) {
		Vector2 mousePos(Game::MouseState.x, Game::MouseState.y);
		/* Iterate backwards through draw queue to locate the lowest level
		element that the mouse occupies*/
		for (int i = m_drawQueue.size() - 1; i >= 0; i--) {
			shared_ptr<Entity> entity;
			shared_ptr<Sprite> sprite = dynamic_pointer_cast<Sprite>(m_drawQueue[i]);
			if (sprite && GuiEM.Find(sprite->ID, entity)) {
				// Note: this assumes styles are not dynamically added
				if (sprite->Rect.Contains(mousePos)) {
					OnHover(entity);
					switch (Game::MouseTracker.leftButton) {
					case ButtonState::PRESSED: OnMouseDown(entity);break;
					case ButtonState::RELEASED: OnMouseUp(entity);break;
					}
					break;
				}
			}
		}
	}
}

void GuiSystem::UpdateUI(int outputWidth, int outputHeight)
{
	m_outputRect = Rectangle(0, 0, outputWidth, outputHeight);
	if (m_currentMenu) {
		shared_ptr<Style> style = m_currentMenu->GetComponent<Style>("Style_Default");
		m_drawQueue = UpdateDrawQueue(CalculateChildRect(m_outputRect,style), m_currentMenu,1);
	}
	else {
		m_drawQueue.clear();
	}
}

void GuiSystem::OpenMenu(string name)
{
	m_currentMenu = GetMenu(name);
	UpdateUI(m_outputRect.width, m_outputRect.height);
}

void GuiSystem::CloseMenu()
{
	m_drawQueue.clear();
	m_currentMenu = nullptr;
}

vector<shared_ptr<Components::Component>> & GuiSystem::GetDrawQueue()
{
	return m_drawQueue;
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

void GuiSystem::AddMenu(string name, EntityPtr menu)
{
	m_menus.insert(std::pair<string, EntityPtr>(name, menu));
}

EntityPtr GuiSystem::GetMenu(string name)
{
	if (m_menus.find(name) != m_menus.end()) {
		return m_menus[name];
	}
	return nullptr;
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

shared_ptr<GUI::Sprite> GuiSystem::UpdateDrawQueue(Rectangle parentRect, EntityPtr entity,int zIndex)
{
	shared_ptr<GUI::Sprite> queue;
	if (entity->HasComponents(GuiEM.ComponentMask(vector<string>{"Style_Default","Sprite"}))) {
		//----------------------------------------------------------------
		// Components
		shared_ptr<Style> style = entity->GetComponent<Style>("Style_Default");
		shared_ptr<Sprite> sprite = entity->GetComponent<Sprite>("Sprite");
		shared_ptr<Text> text = entity->GetComponent<Text>("Text");
		//----------------------------------------------------------------
		// Background Sprite
		UpdateSprite(parentRect, entity, style, sprite, zIndex);
		queue.push_back(sprite);
		//----------------------------------------------------------------
		// Text
		if (text) {
			UpdateText(text, sprite, style);
			queue.ChildSprites.push_back(text);
		}

		// get children
		vector<EntityPtr> children;
		vector<shared_ptr<Style>> childStyles;
		if (entity->HasComponents(GuiEM.ComponentMask("Children"))) {
			for (auto & childID : entity->GetComponent<Children>("Children")->Entities) {
				EntityPtr child;
				if (GuiEM.Find(childID, child) && child->HasComponents(GuiEM.ComponentMask("Style_Default"))) {
					childStyles.push_back(child->GetComponent<Style>("Style_Default"));
					children.push_back(child);
				}
			}
		}
		// recursively render children to queue
		vector<Rectangle> childRects = CalculateChildRects(parentRect, style, childStyles);
		for (int i = 0; i < children.size(); i++) {
			queue.ChildSprites.push_back(UpdateDrawQueue(childRects[i],children[i],zIndex+1));
		}
	}
	return queue;
}

void GuiSystem::OnHover(EntityPtr entity)
{
	if (m_activeElement != entity) {
		if (m_hoverElement && m_hoverElement != entity) OnHoverOut(m_hoverElement);
		if (entity->HasComponents(GuiEM.ComponentMask("Style_Hover"))) {
			shared_ptr<Style> hoverStyle = entity->GetComponent<Style>("Style_Hover");
			shared_ptr<Sprite> sprite = GetSprite(entity);
			UpdateSprite(sprite->Rect, entity, hoverStyle, sprite, sprite->Zindex);
		}
		m_hoverElement = entity;
	}
}

void GuiSystem::OnHoverOut(EntityPtr entity)
{
	if (entity != m_activeElement) {
		OnDefault(entity);
	}
}

void GuiSystem::OnMouseDown(EntityPtr entity)
{
	m_activeElement = entity;
	if (entity->HasComponents(GuiEM.ComponentMask("Style_Active"))) {
		shared_ptr<Style> activeStyle = entity->GetComponent<Style>("Style_Active");
		shared_ptr<Sprite> sprite = GetSprite(entity);
		UpdateSprite(sprite->Rect, entity, activeStyle, sprite, sprite->Zindex);
	}
}

void GuiSystem::OnMouseUp(EntityPtr entity)
{
	if (m_activeElement == entity) {
		OnClick(entity);
	}
	if (m_activeElement) OnDefault(m_activeElement);
	m_activeElement = nullptr;
	OnHover(entity);
}

void GuiSystem::OnClick(EntityPtr entity)
{
	if (entity->HasComponents(GuiEM.ComponentMask("EventHandler_Click"))) {
		shared_ptr<GUI::EventHandler> clickHandler = entity->GetComponent<GUI::EventHandler>("EventHandler_Click");
		if (clickHandler) clickHandler->Callback();
	}
	OnHover(entity);
}

void GuiSystem::OnDefault(EntityPtr entity)
{
	if (entity->HasComponents(GuiEM.ComponentMask(vector<string>{"Style_Default", "Sprite"}))) {
		shared_ptr<Style> defaultStyle = entity->GetComponent<Style>("Style_Default");
		shared_ptr<Sprite> sprite = GetSprite(entity);
		UpdateSprite(sprite->Rect, entity, defaultStyle, sprite, sprite->Zindex);
	}
}

void GuiSystem::UpdateSprite(Rectangle rect, EntityPtr entity, shared_ptr<Style> style, shared_ptr<Sprite> sprite, int zIndex)
{
	sprite->Rect = rect;
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

vector<Rectangle> GuiSystem::CalculateChildRects(Rectangle parentRect, shared_ptr<Style> parentStyle, vector<shared_ptr<Style>> childrenStyles)
{
	FlowType flow = parentStyle->GetFlowDirection();
	vector<Rectangle> childRects;
	int primary = GetPrimaryDimension(flow, parentRect);
	int primaryBisector = primary / 2;
	int secondary = GetSecondaryDimension(flow, parentRect);
	int secondaryBisector = secondary / 2;
	//----------------------------------------------------------------
	// Get total childen length
	int totalChildPrimary = 0;
	int totalChildSecondary = 0;
	for (auto & child : childrenStyles) {
		Rectangle childRect = CalculateChildRect(parentRect, child);
		totalChildPrimary += GetPrimaryDimension(flow, childRect);
		int secondary = GetSecondaryDimension(flow, childRect);
		if (secondary > totalChildSecondary) {
			totalChildSecondary = secondary;
		}
		childRects.push_back(childRect);
	}
	

	//----------------------------------------------------------------
	// Scroll offsets
	Vector2 contentSize = GetVector(flow, totalChildPrimary, totalChildSecondary);
	Vector2 parentSize = GetVector(flow, GetPrimaryDimension(flow, parentRect), GetSecondaryDimension(flow, parentRect));
	Vector2 parentPos = GetVector(flow, GetPrimaryPosition(flow, parentRect), GetSecondaryPosition(flow, parentRect));
	Vector2 contentPos = parentPos - (contentSize - parentSize) * parentStyle->ScrollPosition;
	
	// Create clipping rects
	float topDiff = parentRect.y - contentPos.y;
	float bottomDiff = (contentPos.y + contentSize.y) - (parentRect.y + parentRect.height);
	float leftDiff = parentRect.x - contentPos.x;
	float rightDiff = (contentSize.x + contentPos.x) - (parentRect.x + parentRect.width);

	Rectangle clipTop = Rectangle(contentPos.x, contentPos.y, contentSize.x,topDiff);
	Rectangle clipBottom = Rectangle(contentPos.x, parentRect.y + parentRect.height, contentSize.x, bottomDiff);
	Rectangle clipLeft = Rectangle(contentPos.x, contentPos.y + topDiff, leftDiff, parentRect.height);
	Rectangle clipRight = Rectangle(parentRect.x + parentRect.width, parentRect.y,rightDiff,  parentRect.height);

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
	for (Rectangle & childRect : childRects) {
		// apply scroll offsets
		childRect.x += contentPos.x - parentRect.x;
		childRect.y += contentPos.y - parentRect.y;
		// secondary
		int childSecondaryOffset = secondaryOffset;
		switch (parentStyle->GetAlignItems()) {
		case AlignmentType::Center: childSecondaryOffset += (secondaryBisector - GetSecondaryDimension(flow, childRect) / 2);break;
		case AlignmentType::End: childSecondaryOffset += (secondary - GetSecondaryDimension(flow, childRect));break;
		}
		SetPosition(flow, childRect,primaryOffset, childSecondaryOffset);
		// move the primary offset past this child
		primaryOffset += GetPrimaryDimension(flow, childRect);
	}
	
	return childRects;
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

shared_ptr<Sprite> GuiSystem::GetSprite(EntityPtr entity)
{
	return entity->GetComponent<Sprite>("Sprite");
}
