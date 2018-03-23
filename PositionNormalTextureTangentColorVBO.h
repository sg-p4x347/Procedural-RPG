#pragma once
#include "VBO.h"
namespace Components {
	class PositionNormalTextureTangentColorVBO : 
		public VBO<VertexPositionNormalTangentColorTexture>
	{
	public:
		PositionNormalTextureTangentColorVBO();

		string GetName() override;
	};
}

