#pragma once
#include "Line.h"
#include "JSON.h"

namespace Architecture {
struct Rectangle : public JSON {
	Rectangle();
	Rectangle(const Rectangle & B);
	Rectangle(int xIn, int yIn,int widthIn,int heightIn);
	Rectangle(JsonParser & rect);

	bool IsBordering(Rectangle & B);
	bool IsTouching(Rectangle & B);
	Architecture::Line Touching(Rectangle & B);
	int Area();
	DirectX::SimpleMath::Vector2 Center();
	int Left();
	int Right();
	int Bottom();
	int Top();
	bool operator==(Rectangle & B);
	bool operator!=(Rectangle & B);
	int x;
	int y;
	int width;
	int height;

	// Inherited via JSON
	virtual void Import(JsonParser & jp) override;
	virtual JsonParser Export() override;
};
}