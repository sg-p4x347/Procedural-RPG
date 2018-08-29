#pragma once
#include "pch.h"
#include "ISerialization.h"
#include "JSON.h"
class Entity;
namespace Components {
	class Component :
		public ISerialization
	{
	public:
		Component();
		Component(const unsigned int & id);
		Component(Component & other);
		virtual ~Component();

		
		// The ID of the entity this belongs to
		unsigned int ID;
		// Component name
		virtual string GetName() = 0;
		// Copy
		virtual shared_ptr<Components::Component> Copy(shared_ptr<Components::Component> source);
		// Inherited via ISerialization
		virtual void Import(std::istream & ifs) override;
		virtual void Export(std::ostream & ofs) override;
		
		Entity * Entity;
	};
}

