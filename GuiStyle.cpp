#include "pch.h"
#include "GuiStyle.h"

namespace GUI {

	Style::Style(
		string selector,
		string background, 
		string fontColor,
		string foreground, 
		string width, 
		string height, 
		string flowDirection, 
		string overflowX, 
		string overflowY,
		string justify,
		string alignItems,
		Vector2 scrollPos,

		string font,
		string fontSize,
		string textAlign,
		string verticalTextAlign
	) : Delegate::Delegate(selector) {
		Background = background;
		FontColor = fontColor;
		Foreground = foreground;
		Width = width;
		Height = height;
		FlowDirection = flowDirection;
		OverflowX = overflowX;
		OverflowY = overflowY;
		Justify = justify;
		AlignItems = alignItems;
		ScrollPosition = scrollPos;

		Font = font;
		FontSize = fontSize;
		TextAlign = textAlign;
		VerticalTextAlign = verticalTextAlign;
	}

	Style::~Style()
	{
	}

	ContentType Style::GetBackgroundType()
	{
		if (Background.length() > 4 && Background.substr(Background.length() - 4) == ".png") {
			return ContentType::Image;
		}
		else if (Background.find(":") != string::npos) {
			return ContentType::SpriteSheet;
		}
		return ContentType::Color;
	}

	DirectX::SimpleMath::Color Style::GetBackgroundColor()
	{
		return GetColor(Background);
	}

	string Style::GetBackgroundImage()
	{
		auto colon = Background.find(':');
		if (colon != string::npos) {
			return Background.substr(0, colon);
		}
		return Background;
	}

	string Style::GetBackgroundSprite()
	{
		return Background.substr(Background.find(':')+1);
	}

	DimensionType Style::GetWidth(float & width)
	{
		return GetDimension(Width, width);
	}

	DimensionType Style::GetHeight(float & height)
	{
		return GetDimension(Height, height);
	}

	FlowType Style::GetFlowDirection()
	{
		if (FlowDirection == "column") {
			return FlowType::Column;
		}
		else if (FlowDirection == "row") {
			return FlowType::Row;
		}
		else {
			return FlowType(0);
		}
	}

	AlignmentType Style::GetJustify()
	{
		return GetAlignmentType(Justify);
	}

	AlignmentType Style::GetAlignItems()
	{
		return GetAlignmentType(AlignItems);
	}

	DirectX::SimpleMath::Color Style::GetFontColor()
	{
		return GetColor(FontColor);
	}

	DimensionType Style::GetFontSize(float & size)
	{
		return GetDimension(FontSize,size);
	}

	AlignmentType Style::GetTextAlign()
	{
		return GetAlignmentType(TextAlign);
	}

	AlignmentType Style::GetVerticalTextAlign()
	{
		return GetAlignmentType(VerticalTextAlign);
	}

	OverflowType Style::GetOverflowX()
	{
		return GetOverflow(OverflowX);
	}

	OverflowType Style::GetOverflowY()
	{
		return GetOverflow(OverflowY);
	}

	Style Style::operator+(Style & other)
	{
		// TEMP
		// default values should be "", and this should check to see if one has it and not the other, vs specific cases
		return Style(
			DelegateName,
			Background == "" ? other.Background : Background,
			FontColor == "rgb(0,0,0)" ? other.FontColor : FontColor,
			Foreground == "" ? other.Foreground : Foreground,
			Width == "100%" ? other.Width : Width,
			Height == "100%" ? other.Height : Height,
			FlowDirection == "column" ? other.FlowDirection : FlowDirection,
			OverflowX == "scroll" ? other.OverflowX : OverflowX,
			OverflowY == "scroll" ? other.OverflowY : OverflowY,
			Justify == "start" ? other.Justify : Justify,
			other.AlignItems == "start" ? other.AlignItems : other.AlignItems
		);
	}

	AlignmentType Style::GetAlignmentType(string value)
	{
		if (value == "start") {
			return AlignmentType::Start;
		}
		else if (value == "center") {
			return AlignmentType::Center;
		}
		else if (value == "end") {
			return AlignmentType::End;
		}
		else {
			return AlignmentType(0);
		}
	}
	DimensionType Style::GetDimension(string value, float & dimension)
	{
		std::stringstream stream(value);
		if (!stream.eof()) {
			stream >> dimension;
			if (!stream.eof()) {
				string suffix = "";
				stream >> suffix;
				if (suffix == "px") {
					return DimensionType::Pixel;
				}
				else if (suffix == "%") {
					dimension /= 100;
					return DimensionType::Percent;
				}
			}
		}
		dimension = 0.f;
		return DimensionType(0);
	}
	DirectX::SimpleMath::Color Style::GetColor(string value)
	{
		if (value != "") {
			std::stringstream stream(value);
			float values[4];
			for (int i = 0; i < 4; i++) {
				while (!stream.eof() && !isdigit(stream.peek())) stream.ignore();
				if (stream.eof() || !(stream >> values[i])) {
					values[i] = 1.f;
				}
			}
			return DirectX::SimpleMath::Color(values);
		}
		return DirectX::Colors::Transparent;
	}
	OverflowType Style::GetOverflow(string value)
	{
		if (value == "scroll") {
			return OverflowType::Scroll;
		}
		else if (value == "hidden") {
			return OverflowType::Hidden;
		}

	}
	string Style::GetDiscreteName()
	{
		return "Style";
	}
}
