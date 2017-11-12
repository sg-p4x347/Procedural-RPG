#pragma once
#include "ISerialization.h"
#include "JSON.h"
#include "Filesystem.h"

namespace Components {
	class Component :
		public ISerialization
	{
	public:
		
		Component(const unsigned int & id);
		Component(Component & other);
		virtual ~Component();

		// Sets the parent directory for all components
		static void SetDirectory(Filesystem::path directory);
		// Getst he parent directory for all components
		static Filesystem::path GetDirectory();
		// saves this component to file
		void Save(Filesystem::path directory); 
		// returns a concrete component from filestream
		void Load(Filesystem::path directory, const unsigned int & id);
		
		// The ID of the entity this belongs to
		unsigned int ID;
		// Component name
		virtual string GetName() = 0;

		// Inherited via ISerialization
		virtual void Import(std::ifstream & ifs) override;
		virtual void Export(std::ofstream & ofs) override;
		
	protected:
		// File IO streams
		std::ofstream GetOutputStream(Filesystem::path directory, const unsigned int & id);
		std::ifstream GetInputStream(Filesystem::path directory, const unsigned int & id);
	private:
		static Filesystem::path m_directory;
	};
}

