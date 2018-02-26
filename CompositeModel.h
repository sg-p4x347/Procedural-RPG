#pragma once
class CompositeModel
{
public:
	CompositeModel();
	void AddMesh(shared_ptr<ModelMesh> & mesh, Vector3 & position, Vector3 & rotation);
	void AddMesh(shared_ptr<ModelMesh> & mesh, XMMATRIX & matrix);
	vector<std::pair<shared_ptr<ModelMesh>, XMMATRIX>> meshes;
};

