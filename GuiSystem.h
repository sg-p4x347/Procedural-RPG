#pragma once
#include "System.h"
#include "GuiEntityManager.h"
#include "Sprite.h"
#include "GuiText.h"
using namespace GUI;
class GuiSystem :
	public System
{
public:
	GuiSystem(
		unsigned short updatePeriod);
	~GuiSystem();

	// Inherited via System
	virtual void SyncEntities() override;
	virtual string Name() override;
	virtual void Update(double & elapsed) override;
	// Window resize update
	void UpdateUI(int outputWidth, int outputHeight);

	void OpenMenu(string name);
	void CloseMenu();
	//----------------------------------------------------------------
	// Draw queue for RenderSystem
	vector<shared_ptr<Components::Component>> & GetDrawQueue();
private:
	GUI::GuiEntityManager GuiEM;
	Rectangle m_outputRect;
	//----------------------------------------------------------------
	// Menus
	EntityPtr m_currentMenu;
	map<string, EntityPtr> m_menus;
	void AddMenu(string name, EntityPtr menu);
	EntityPtr GetMenu(string name);
	

	//----------------------------------------------------------------
	// Spritesheets
	map<string, map<string, Rectangle>> m_spriteSheets;
	void AddSpriteSheet(string path, map<string, Rectangle> mapping);
	Rectangle GetSpriteRect(string filePath, string spriteName);
	//----------------------------------------------------------------
	// Style Updates
	EntityPtr m_activeElement;
	EntityPtr m_hoverElement;
	void UpdateSprite(Rectangle rect, EntityPtr entity, shared_ptr<Style> style, shared_ptr<Sprite> sprite, int zIndex);
	void UpdateText(shared_ptr<Text> text, shared_ptr<Sprite> sprite, shared_ptr<Style> style);
	//----------------------------------------------------------------
	// Events
	void OnHover(EntityPtr entity);
	void OnHoverOut(EntityPtr entity);
	void OnMouseDown(EntityPtr entity);
	void OnMouseUp(EntityPtr entity);
	void OnClick(EntityPtr entity);

	void OnDefault(EntityPtr entity);
	//----------------------------------------------------------------
	// View engine
	vector<shared_ptr<Components::Component>> m_drawQueue;
	vector<shared_ptr<Components::Component>> UpdateDrawQueue(Rectangle parentRect, EntityPtr entity, int zIndex);
	vector<Rectangle> CalculateChildRects(Rectangle parentRect, shared_ptr<Style> parentStyle,vector<shared_ptr<Style>> childrenStyles);

	Rectangle CalculateChildRect(Rectangle parentRect, shared_ptr<Style> childStyle);
	void SetPosition(FlowType flow, Rectangle & rect, int primary, int secondary);

	int GetPrimaryDimension(FlowType flow, Rectangle rect);
	int GetSecondaryDimension(FlowType flow, Rectangle rect);

	int GetPrimaryPosition(FlowType flow, Rectangle rect);
	int GetSecondaryPosition(FlowType flow, Rectangle rect);

	//----------------------------------------------------------------
	// Misc helpers
	shared_ptr<Sprite> GetSprite(EntityPtr entity);
};

