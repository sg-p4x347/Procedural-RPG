#include "pch.h"
#include "PositionNormalTextureVBO.h"

namespace Components {

	PositionNormalTextureVBO::PositionNormalTextureVBO() : VBO<VertexPositionNormalTangentColorTexture>::VBO()
	{
	}

	string PositionNormalTextureVBO::GetName()
	{
		return "PositionNormalTextureVBO";
	}
}
