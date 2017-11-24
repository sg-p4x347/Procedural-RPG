#include "pch.h"
#include "Line.h"
#include "Rectangle.h"
#include "JSON.h"

using namespace std;
using namespace Architecture;

namespace Architecture {
	Rectangle::Rectangle() {
	}
	Rectangle::Rectangle(const Rectangle & B)
	{
		x = B.x;
		y = B.y;
		width = B.width;
		height = B.height;
	}
	Rectangle::Rectangle(int xIn, int yIn, int widthIn, int heightIn)
	{
		x = xIn;
		y = yIn;
		width = widthIn;
		height = heightIn;
	}
	Rectangle::Rectangle(JsonParser & rect)
	{
		Import(rect);
	}
	bool Rectangle::IsBordering(Rectangle & B)
	{
		bool xBordering = (Left() == B.Left() || Right() == B.Right());
		bool yBordering = (Bottom() == B.Bottom() || Top() == B.Top());
		return xBordering || yBordering;
	}
	bool Rectangle::IsTouching(Rectangle & B)
	{
		/*Line common = Touching(B);
		for (Line edge : Edges()) {
			if (common.Overlap(edge).Length() > 0) return true;
		}*/
		bool xAdjacent = Right() == B.Left() || B.Right() == Left();
		bool yAdjacent = Top() == B.Bottom() || Bottom() == B.Top();
		bool xOverlap = min(Right(), B.Right()) - max(Left(), B.Left()) > 0;
		bool yOverlap = min(Top(), B.Top()) - max(Bottom(), B.Bottom()) > 0;
		return (xAdjacent && yOverlap) || (yAdjacent && xOverlap);
	}
	Architecture::Line Rectangle::Touching(Rectangle & B)
	{
		Vector3 pA, pB;
		pA.x = max(x, B.x);
		pB.x = min(x + width, B.x + B.width);

		pA.y = max(y, B.y);
		pB.y = min(y + height, B.y + B.height);
		return Architecture::Line(pA, pB);

	}
	int Rectangle::Area()
	{
		return width * height;
	}
	Vector2 Rectangle::Center()
	{
		return Vector2(x+ width/2,y + height/2);
	}
	int Rectangle::Left()
	{
		return x;
	}
	int Rectangle::Right()
	{
		return x + width;
	}
	int Rectangle::Bottom()
	{
		return y;
	}
	int Rectangle::Top()
	{
		return y + height;
	}
	bool Rectangle::operator==(Rectangle & B)
	{
		return x == B.x && y == B.y && width == B.width && height == B.height;
	}
	bool Rectangle::operator!=(Rectangle & B)
	{
		return !(*this == B);
	}
	void Rectangle::Import(JsonParser & rectangle)
	{
		x = rectangle["x"].To<short>();
		y = rectangle["y"].To<short>();
		width = rectangle["width"].To<short>();
		height = rectangle["height"].To<short>();
	}
	JsonParser Rectangle::Export()
	{
		JsonParser rectangle;
		rectangle.Set("x", x);
		rectangle.Set("y", y);
		rectangle.Set("width", width);
		rectangle.Set("height", height);
		return rectangle;
	}
}