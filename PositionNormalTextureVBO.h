#pragma once
#include "VBO.h"
namespace Components {
	class PositionNormalTextureVBO :
		public VBO<VertexPositionNormalTexture>
	{
	public:
		PositionNormalTextureVBO();

		string GetName() override;
	};
}

