#pragma once
#include "pch.h"
#include "ISerialization.h"
#include "JSON.h"
namespace Components {
	class Component :
		public ISerialization
	{
	public:
		Component();
		Component(const unsigned int & id);
		Component(Component & other);
		virtual ~Component();

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

