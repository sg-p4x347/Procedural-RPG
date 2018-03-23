#include "pch.h"
#include "PositionNormalTextureVBO.h"

namespace Components {

	PositionNormalTextureVBO::PositionNormalTextureVBO() : VBO<VertexPositionNormalTexture>::VBO()
	{
	}

	string PositionNormalTextureVBO::GetName()
	{
		return "PositionNormalTextureVBO";
	}
}
