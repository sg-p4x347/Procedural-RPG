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
		string overflow, 
		string justify,
		string alignItems,

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
		Overflow = overflow;
		Justify = justify;
		AlignItems = alignItems;

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
	string Style::GetDiscreteName()
	{
		return "Style";
	}
}
