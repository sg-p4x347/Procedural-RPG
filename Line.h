#pragma once

using namespace std;
using namespace DirectX::SimpleMath;

namespace Architecture {
class Line
{
public:
	Line(Vector3 & A, Vector3 & B);
	double Length();
	Line Overlap(Line & B);
	bool CoLinear(Line & B);
	bool SharedCoord(int x);
private:
	pair<Vector3,Vector3> m_points;
};
}

