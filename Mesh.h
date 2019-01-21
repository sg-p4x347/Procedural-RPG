#pragma once
#include "MeshPart.h"
#include "ISerialization.h"
namespace geometry {
	class Mesh :
		public ISerialization
	{
	public:
		Mesh();
		~Mesh();
		void AddPart(MeshPart & part);
		vector<MeshPart> & GetParts();
		// Inherited via ISerialization
		virtual void Import(std::istream & ifs) override;
		virtual void Export(std::ostream & ofs) override;
	private:
		vector<MeshPart> m_parts;

		
	};
}
