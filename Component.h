#pragma once
#include "ISerialization.h"
#include "JSON.h"
#include "Filesystem.h"

using string = std::string;
namespace Components {
	class Component :
		public JSON,
		public ISerialization
	{
	public:
		Component();
		Component(Component & other);
		virtual ~Component() { };
		// Called from the prototype
		virtual shared_ptr<Component> GetComponent(const unsigned int & id) = 0; // implicitly loads untracked components
		virtual void Save(string directory); // saves this component to file
		virtual shared_ptr<Component> Load(string directory, const unsigned int & id); // returns a concrete component from filestream
		virtual void SaveAll(string directory) = 0; // saves all components
		virtual shared_ptr<Component> Add(const unsigned int & id) = 0;
		// The ID of the entity this belongs to
		unsigned int ID;
		// Component name
		virtual string GetName() = 0;
		// Inherited via JSON
		virtual void Import(JsonParser & jp) override;
		virtual JsonParser Export() override;

		// Inherited via ISerialization
		virtual void Import(std::ifstream & ifs) override;
		virtual void Export(std::ofstream & ofs) override;
	protected:
		// Constructor
		Component(const unsigned int & id);

		// File IO streams
		std::ofstream GetOutputStream(string directory, const unsigned int & id);
		std::ifstream GetInputStream(string directory, const unsigned int & id);

		
	};
}

