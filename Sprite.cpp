#include "pch.h"
#include "Sprite.h"

namespace GUI {
	Sprite::Sprite(
		SimpleMath::Color backgroundColor,
		string backgroundImage,
		Rectangle rect,
		int zIndex,
		bool useSourceRect,
		Rectangle sourceRect,
		vector<Rectangle> clippingRects,
		bool visible
	) : Components::Component(0)
		

	{
		BackgroundColor = backgroundColor;
		BackgroundImage = backgroundImage;
		Rect = rect;
		Zindex = zIndex;
		UseSourceRect = useSourceRect;
		SourceRect = sourceRect;
		ClippingRects = clippingRects;
		Visible = visible;
		ScrollPosition = Vector2::Zero;
	}

	string Sprite::GetName()
	{
		return "Sprite";
	}
	shared_ptr<Components::Component> Sprite::Copy(shared_ptr<Components::Component> source)
	{
		return make_shared<GUI::Sprite>(*dynamic_pointer_cast<GUI::Sprite>(source));
	}
}
