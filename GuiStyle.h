#pragma once
#include "Delegate.h"

namespace GUI {
	enum ContentType {
		Color,
		Image,
		SpriteSheet
	};
	enum DimensionType {
		Pixel,
		Percent
	};
	enum FlowType {
		Column,
		Row
	};
	enum OverflowType {
		Hidden,
		Scroll
	};
	enum AlignmentType {
		Start,
		Center,
		End
	};
	class Style :
		public Components::Delegate
	{
	public:
		Style(
			string selector = "Default",
			string background = "",
			string fontColor = "rgb(0,0,0)",
			string foreground = "",
			string width = "100%",
			string height = "100%",
			string flowDirection = "column",
			string overflowX = "scroll",
			string overflowY = "scroll",
			string justify = "start",
			string alignItems = "start",
			Vector2 scrollPos = Vector2::Zero,

			string font = "impact",
			string fontSize = "32px",
			string textAlign = "start",
			string verticalTextAlign = "center"
		);
		~Style();
		/*
			-------------
			 Style Types
			-------------
			Color: rgb(0,0.5,1.0) | rgba(0,0,0,0.5)
			Image: example.png
			Dimension: 12px | 12%
		*/
		// Box model attributes
		string Background;		// Color | Image
		string FontColor;		// Color
		string Foreground;		// Color | Image
		string Width;			// Dimension
		string Height;			// Dimension
		string FlowDirection;	// column | row
		string OverflowX;		// hidden | scroll
		string OverflowY;		// hidden | scroll
		string Justify;			// start | center | end
		string AlignItems;		// start | center | end
		Vector2 ScrollPosition;	// [0.f -> 1.f] in both directions
		// Text
		string Font;
		string FontSize;
		string TextAlign;
		string VerticalTextAlign;
		


		//----------------------------------------------------------------
		// Accessors
		ContentType GetBackgroundType();
		DirectX::SimpleMath::Color GetBackgroundColor();
		string GetBackgroundImage();
		string GetBackgroundSprite();

		DimensionType GetWidth(float & width);
		DimensionType GetHeight(float & height);

		FlowType GetFlowDirection();
		AlignmentType GetJustify();
		AlignmentType GetAlignItems();

		DirectX::SimpleMath::Color GetFontColor();
		DimensionType GetFontSize(float & size);
		AlignmentType GetTextAlign();
		AlignmentType GetVerticalTextAlign();

		OverflowType GetOverflowX();
		OverflowType GetOverflowY();
		//----------------------------------------------------------------
		// Operators
		Style operator+(Style & other);
	private:
		AlignmentType GetAlignmentType(string value);
		DimensionType GetDimension(string value, float & dimension);
		DirectX::SimpleMath::Color GetColor(string value);
		OverflowType GetOverflow(string value);
		// Inherited via Delegate
		virtual string GetDiscreteName() override;
	};
}

