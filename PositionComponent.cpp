#include "pch.h"
#include "PositionComponent.h"

PositionComponent::PositionComponent(unsigned int id, XMFLOAT3 pos, XMFLOAT3 rot)
{
	ID = id;
	position = pos;
	rotation = rot;
}

PositionComponent::~PositionComponent()
{
}
