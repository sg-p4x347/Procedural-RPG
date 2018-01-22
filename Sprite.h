#pragma once
#include "Component.h"
namespace GUI {
	class Sprite :
		public Components::Component
	{
	public:
		Sprite(
			SimpleMath::Color backgroundColor = SimpleMath::Color(Colors::Transparent),
			string backgroundImage = "",
			Rectangle rect = Rectangle(0,0,0,0),
			int zIndex = 1,
			bool useSourceRect = false,
			Rectangle sourceRect = Rectangle()
		);
		SimpleMath::Color BackgroundColor;
		string BackgroundImage;
		Rectangle Rect;
		int Zindex;
		EntityPtr SourceEntity;
		Rectangle SourceRect;
		bool UseSourceRect;

		// Inherited via Component
		virtual string GetName() override;
	};
}

