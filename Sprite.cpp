#include "pch.h"
#include "Sprite.h"

namespace GUI {
	Sprite::Sprite(
		SimpleMath::Color backgroundColor,
		string backgroundImage,
		Rectangle rect,
		int zIndex,
		bool useSourceRect,
		Rectangle sourceRect
	) : Components::Component(0)
		

	{
		BackgroundColor = backgroundColor;
		BackgroundImage = backgroundImage;
		Rect = rect;
		Zindex = zIndex;
		UseSourceRect = useSourceRect;
		SourceRect = sourceRect;
	}

	string Sprite::GetName()
	{
		return "Sprite";
	}
}