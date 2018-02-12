#pragma once
#include "VBO.h"
namespace Components {
	class PositionNormalTextureVBO :
		public VBO<VertexPositionNormalTangentColorTexture>
	{
	public:
		PositionNormalTextureVBO();

		string GetName() override;
	};
}

