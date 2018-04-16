#pragma once
#include "Component.h"

namespace GUI {
	class Text :
		public Components::Component
	{
	public:
		Text(string text = "",string font = "",Vector2 position = Vector2::Zero,int fontSize=0,SimpleMath::Color color=SimpleMath::Color(Colors::Black));
		// Data
		string String;
		string Font;
		Vector2 Position;
		int FontSize;
		SimpleMath::Color Color;

		// Inherited via Component
		virtual string GetName() override;

		// Inherited via Component
		virtual shared_ptr<Components::Component> Copy(shared_ptr<Components::Component> source) override;
	};
}

