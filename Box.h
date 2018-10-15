#pragma once
struct Box
{
	Box();
	Box(Vector3 position, Vector3 size);
	Vector3 Position;
	Vector3 Size;

	bool Contains(Vector3 point);
	
};

