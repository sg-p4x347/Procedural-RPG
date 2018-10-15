#include "pch.h"
#include "Box.h"


Box::Box()
{
}

Box::Box(Vector3 position, Vector3 size) : Position(position), Size(size)
{
}

bool Box::Contains(Vector3 point)
{
	return (Position.x <= point.x && Position.x + Size.x >= point.x
		&& Position.y <= point.y && Position.y + Size.y >= point.y
		&& Position.z <= point.z && Position.z + Size.z >= point.z);
}
