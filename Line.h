#pragma once

struct Line
{
	Line(Vector3 a, Vector3 b);
	Vector3 A;
	Vector3 B;
	double Length();
	//Line Overlap(Line & B);
	//bool CoLinear(Line & B);
	bool SharedCoord(int x);
};


