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
		vector<Components::Component> childSprites
	) : Components::Component(0)
		

	{
		BackgroundColor = backgroundColor;
		BackgroundImage = backgroundImage;
		Rect = rect;
		Zindex = zIndex;
		UseSourceRect = useSourceRect;
		SourceRect = sourceRect;
		ClippingRects = clippingRects;
		ChildSprites = childSprites;
	}

	string Sprite::GetName()
	{
		return "Sprite";
	}
}
