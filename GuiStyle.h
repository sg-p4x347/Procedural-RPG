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
	enum class VisibilityType {
		Visible,
		Hidden
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
			string width2 = "0px",
			string height2 = "0px",
			string flowDirection = "column",
			string overflowX = "hidden",
			string overflowY = "hidden",
			string justify = "start",
			string alignItems = "start",

			string font = "impact",
			string fontSize = "32px",
			string textAlign = "start",
			string verticalTextAlign = "center",

			string visibility = "visible",
			bool wrapItems = false
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
		// Display properties
		string Background;		// Color | Image
		string Foreground;		// Color | Image
		string Visibility;		// Visible | Hidden

		// Box model attributes
		string Width;			// Dimension
		string Height;			// Dimension
		string Width2;			// Dimension
		string Height2;			// Dimension
		string FlowDirection;	// column | row
		string OverflowX;		// hidden | scroll
		string OverflowY;		// hidden | scroll
		string Justify;			// start | center | end
		string AlignItems;		// start | center | end
		bool WrapItems;			// if true, items will wrap into rows rather than overflowing across the primary axis
		// Text
		string Font;
		string FontColor;		// Color
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
		DimensionType GetWidth2(float & width);

		DimensionType GetHeight(float & height);
		DimensionType GetHeight2(float & height);

		FlowType GetFlowDirection();
		AlignmentType GetJustify();
		AlignmentType GetAlignItems();

		DirectX::SimpleMath::Color GetFontColor();
		DimensionType GetFontSize(float & size);
		AlignmentType GetTextAlign();
		AlignmentType GetVerticalTextAlign();

		OverflowType GetOverflowX();
		OverflowType GetOverflowY();

		VisibilityType GetVisibility();
		//----------------------------------------------------------------
		// Operators
		Style operator+(Style & other);
		void operator=(Style & other);
	private:
		AlignmentType GetAlignmentType(string value);
		DimensionType GetDimension(string value, float & dimension);
		DirectX::SimpleMath::Color GetColor(string value);
		OverflowType GetOverflow(string value);
		// Inherited via Delegate
		virtual string GetDiscreteName() override;

		// Inherited via Delegate
		virtual shared_ptr<Components::Component> Copy(shared_ptr<Components::Component> source) override;
	};
}

