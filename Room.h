#pragma once
#include "Rectangle.h"
namespace Architecture {
	
	class Room {
	public:
		//=================================================
		// Properties
		//=================================================
		Architecture::Rectangle rect;
		string type;
		vector<shared_ptr<Room>> links;
		JsonParser config;
		//=================================================
		// Methods
		//=================================================
		Room();
		Room(const Room & other);
		Room(Rectangle & r, const JsonParser & config);
		// Room networking
		bool IsLinkedWith(const  Room * B) const;
		bool HasLinkTo(const  Room * B) const;
		bool CanLinkWith(const  Room * B) const;
	};
}
