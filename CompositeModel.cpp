#include "pch.h"
#include "CompositeModel.h"


CompositeModel::CompositeModel()
{
}

void CompositeModel::AddMesh(shared_ptr<ModelMesh> & mesh, Vector3 & position, Vector3 & rotation)
{
	XMMATRIX translation = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotMat = XMMatrixRotationRollPitchYawFromVector(rotation);
	XMMATRIX final = XMMatrixMultiply(rotMat, translation);
	meshes.push_back(std::make_pair(mesh,final));
}

void CompositeModel::AddMesh(shared_ptr<ModelMesh>& mesh, XMMATRIX & matrix)
{
	meshes.push_back(std::make_pair( mesh, matrix));
}
