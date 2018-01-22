#include "pch.h"
#include "GuiText.h"

namespace GUI {
	Text::Text(string text, string font, Vector2 position, int fontSize, SimpleMath::Color color)
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
