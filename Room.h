#pragma once
#include "Rectangle.h"

namespace Architecture {
	struct Room {
		//=================================================
		// Properties
		//=================================================
		Architecture::Rectangle rect;
		string type;
		vector<Room *> links;
		JsonParser config;
		//=================================================
		// Methods
		//=================================================
		Room();
		Room(const Room & other);
		Room(Rectangle & r, const JsonParser & config);
		// Room networking
		bool IsLinkedWith(const Room * B) const;
		bool HasLinkTo(const Room * B) const;
		bool CanLinkWith(const Room * B) const;
	};
}
