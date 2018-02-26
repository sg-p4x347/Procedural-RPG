#include "pch.h"
#include "Rectangle.h"
#include "JsonParser.h"
#include "Room.h"


namespace Architecture {
	Room::Room() : rect(), config()
	{
	}
	Room::Room(const Room & other) : rect(other.rect), config(other.config) 
	{
		links = other.links;
		type = other.type;
	}
	Room::Room(Rectangle & r, const JsonParser & c) : rect(r), config(c)
	{
		type = config["type"].To<string>();
	}
	bool Room::IsLinkedWith(const Room * B) const
	{
		return (HasLinkTo(B) || B->HasLinkTo(this));
	}
	bool Room::HasLinkTo(const Room * B) const
	{
		for (shared_ptr<Room> A : links) {
			if (A.get() == B) return true;
		}
		return false;
	}
	bool Room::CanLinkWith(const Room * B) const
	{
		return (links.size() < config["maxLinks"].To<int>() && config["linksTo"].IndexOf<string>(B->type) >= 0)
			|| (B->links.size() < B->config["maxLinks"].To<int>() && B->config["linksTo"].IndexOf<string>(type) >= 0);
	}
}
