#pragma once
#include "Mesh.h"
namespace geometry {
	class CMF
	{
	public:
		CMF();
		~CMF();
		static shared_ptr<CMF> CreateFromFBX(string path);
	private:
		vector<Mesh> m_meshes;
	};
}
