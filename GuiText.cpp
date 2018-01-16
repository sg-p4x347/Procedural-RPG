#include "pch.h"
#include "GuiText.h"

namespace GUI {
	Text::Text(const unsigned int & id) : Components::Component(id)
	{
	}
	Text::Text(string text, string font, Vector2 position, int fontSize, SimpleMath::Color color) : Components::Component(0)
	{
		String = text;
		Font = font;
		Position = position;
		FontSize = fontSize;
		Color = color;
	}
	string Text::GetName()
	{
		return "Text";
	}
}
