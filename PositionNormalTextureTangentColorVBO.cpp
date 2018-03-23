#include "pch.h"
#include "PositionNormalTextureTangentColorVBO.h"
namespace Components {

	PositionNormalTextureTangentColorVBO::PositionNormalTextureTangentColorVBO() : VBO<VertexPositionNormalTangentColorTexture>::VBO()
	{
	}

	string Components::PositionNormalTextureTangentColorVBO::GetName()
	{
		return "PositionNormalTextureTangentColorVBO";
	}
}
