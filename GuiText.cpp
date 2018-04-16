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
	shared_ptr<Components::Component> Text::Copy(shared_ptr<Components::Component> source)
	{
		return make_shared<GUI::Text>(*dynamic_pointer_cast<GUI::Text>(source));
	}
}
