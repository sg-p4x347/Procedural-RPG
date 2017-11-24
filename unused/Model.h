#pragma once
#include "Component.h"
namespace Component {
	class Model :
		public Component
	{
	public:
		Model(const unsigned int & id);
		Model(std::ifstream & ifs);

		// override base serialization
		virtual void Import(std::ifstream & ifs) override;
		virtual void Export(std::ofstream & ofs) override;

		vector<Model> & GetComponents();
		// Inherited via Component
		virtual shared_ptr<Component> GetComponent(const unsigned int & id) override;
		virtual void Attach(shared_ptr<Component> &component) override;
		virtual void SaveAll(string directory) override;

		// Model
		unsigned short ModelID;
		// Texture
		unsigned short TextureID;
	};
}
