#include "pch.h"
#include "Distribution.h"

using namespace DirectX::SimpleMath;

// Initilization
Distribution::Distribution() {
	Distribution(256, 256);
}
Distribution::Distribution(const unsigned int width, const unsigned int height) {
	m_width = width;
	m_height = height;
	m_points = vector< vector<float> >(m_width + 1, vector<float>(m_height + 1));
}
// Diamond Square distribution algorithm
void Distribution::DiamondSquare (float maxDeviation, const float deviationDecrease,const int zoom,const bool isMountain) {
	m_maxDeviation = maxDeviation;
	m_deviationDecrease = deviationDecrease;
	m_zoom = zoom;
	m_mountain = isMountain;
	// initialize the corners
	//float corners = isMountain ? -m_maxDeviation : deviation(maxDeviation);
	m_points[0][0] = isMountain ? -m_maxDeviation : Deviation(maxDeviation);
	m_points[0][m_width] = isMountain ? -m_maxDeviation : Deviation(maxDeviation);
	m_points[m_width][0] = isMountain ? -m_maxDeviation : Deviation(maxDeviation);
	m_points[m_width][m_width] = isMountain ? -m_maxDeviation : Deviation(maxDeviation);
	// iterate
	for (int iteration = 0; iteration < floor(log2(m_width)); iteration++) {
		int gridWidth = (m_width / pow(2, iteration))/2;
		for (int x = gridWidth; x < m_width; x += gridWidth*2) {
			for (int y = gridWidth; y < m_width; y += gridWidth*2) {
				// Diamond
				m_points[x][y] = isMountain && iteration == 0 ? m_maxDeviation : Diamond(x, y, gridWidth) + Deviation(maxDeviation);
				// Square
				for (float rad = 0; rad <= 4; rad += 1) {
					int pointX = round(x + cos(rad*XM_PI / 2) * gridWidth);
					int pointY = round(y + sin(rad*XM_PI / 2) * gridWidth);
					if (isMountain && (pointX == 0 || pointX == m_width || pointY == 0 || pointY == m_width)) {
						m_points[pointX][pointY] = -m_maxDeviation;
					} else {
						m_points[pointX][pointY] = Square(pointX, pointY, gridWidth) + Deviation(maxDeviation);
					}
				}
			}
		}
		// decrease the random deviation range
		if (iteration >= zoom) {
			maxDeviation -= deviationDecrease * maxDeviation;
		}
	}
}
// continent generator
void Distribution::Continent(int ctrlWidth) {
	// create the base continent map
	unique_ptr<Distribution> continentDist(new Distribution(m_width, m_height));
	continentDist->DiamondSquare(m_width / 8, 0.55f, 2, true);
	vector< vector<float> > continentMap = continentDist->GetPoints();
	// create the biome distribution map
	unique_ptr<Distribution> biomeDist(new Distribution(m_width, m_height));
	biomeDist->DiamondSquare(m_width / 16, 1.0f, 3, false);
	vector< vector<float> > biomeMap = biomeDist->GetPoints();

	m_maxDeviation = m_width / 16;
	float maxDeviation = m_maxDeviation;
	m_deviationDecrease = 0.55f;
	// amplitude
	const float landAmplitude = 0.2;
	const float oceanAmplitude = 0.6;
	const float biomeAmplitude = 16;
	const float offset = 32;
	// initialize the corners
	m_points[0][0] = continentMap[0][0] * oceanAmplitude;
	m_points[0][m_width] = continentMap[0][m_width] * oceanAmplitude;
	m_points[m_width][0] = continentMap[m_width][0] * oceanAmplitude;
	m_points[m_width][m_width] = continentMap[m_width][m_width] * oceanAmplitude;
	// iterate
	for (int iteration = 0; iteration < floor(log2(m_width)); iteration++) {
		int gridWidth = (m_width / pow(2, iteration)) / 2;
		for (int x = gridWidth; x < m_width; x += gridWidth * 2) {
			for (int y = gridWidth; y < m_width; y += gridWidth * 2) {
				// Diamond
				float points[4][4];
				if (gridWidth >= ctrlWidth) {
					if (gridWidth >= 1) {
						m_points[x][y] = continentMap[x][y]+offset;
						if (m_points[x][y] > 0) {
							m_points[x][y] *= landAmplitude;
							if (m_points[x][y] >= 0) {
								m_points[x][y] += BiomeDeviation(biomeMap[x][y], iteration) * biomeAmplitude;
							}
							
						}
						else {
							m_points[x][y] *= oceanAmplitude;
							m_points[x][y] += Deviation(maxDeviation) * (BiomeDeviation(biomeMap[x][y], iteration) * biomeAmplitude);
						}
					}
					else {
						m_points[x][y] = Diamond(x, y, gridWidth) + Deviation(maxDeviation) * (BiomeDeviation(biomeMap[x][y], iteration) * biomeAmplitude);
					}
				} else {
					// new way
					// interpolate
					// https://en.wikipedia.org/wiki/Bicubic_interpolation#Bicubic_convolution_algorithm
					// find the 16 control points that define the interpolation
					for (int j = -1; j < 3; j++) {
						for (int i = -1; i < 3; i++) {
							int gridX = i*ctrlWidth + floor(x / ctrlWidth)*ctrlWidth;
							int gridY = j*ctrlWidth + floor(y / ctrlWidth)*ctrlWidth;
							if (gridX >= 0 && gridX <= m_width && gridY >= 0 && gridY <= m_height) {
								points[i + 1][j + 1] = m_points[gridX][gridY];
							} else {
								points[i + 1][j + 1] = -m_maxDeviation * oceanAmplitude;
							}
						}
					}
					m_points[x][y] = BicubicInterpolate(points, (x - (floor(x / ctrlWidth)*ctrlWidth)) / ctrlWidth, (y - (floor(y / ctrlWidth)*ctrlWidth)) / ctrlWidth);
				}
				// Square
				if (gridWidth >= ctrlWidth) {
					for (float rad = 0; rad <= 4; rad += 1) {
						int pointX = round(x + cos(rad*XM_PI / 2) * gridWidth);
						int pointY = round(y + sin(rad*XM_PI / 2) * gridWidth);
						if (gridWidth >= 1) {
							m_points[pointX][pointY] = continentMap[pointX][pointY]+offset;
							if (m_points[pointX][pointY] > 0) {
								m_points[pointX][pointY] *= landAmplitude;
								if (m_points[pointX][pointY] >= 0) {
									m_points[pointX][pointY] += BiomeDeviation(biomeMap[pointX][pointY], iteration) * biomeAmplitude;
								}
								m_points[pointX][pointY] += (Deviation(maxDeviation) * (BiomeDeviation(biomeMap[pointX][pointY], iteration) * biomeAmplitude));
							}
							else {
								m_points[pointX][pointY] *= oceanAmplitude;
							}
							
						}
						else {
							m_points[pointX][pointY] = Square(pointX, pointY, gridWidth) + (Deviation(maxDeviation) * (BiomeDeviation(biomeMap[pointX][pointY], iteration) * biomeAmplitude * 1.5));
						}
					}
				} else {
					for (float rad = 0; rad <= 4; rad += 1) {
						int pointX = round(x + cos(rad*XM_PI / 2) * gridWidth);
						int pointY = round(y + sin(rad*XM_PI / 2) * gridWidth);
						m_points[pointX][pointY] = BicubicInterpolate(points, (pointX - (floor(pointX / ctrlWidth)*ctrlWidth)) / ctrlWidth, (pointY - (floor(pointY / ctrlWidth)*ctrlWidth)) / ctrlWidth);
					}
				}
			}
		}
		// decrease the random deviation range
		maxDeviation -= m_deviationDecrease * maxDeviation;
	}
	// Erosion filter
	//Erosion();
}
// Diamond
float Distribution::Diamond(int x, int y, int distance) {
	float sum = 0;
	sum += m_points[x - distance][y - distance];
	sum += m_points[x - distance][y + distance];
	sum += m_points[x + distance][y - distance];
	sum += m_points[x + distance][y + distance];
	return sum / 4;
}
// Square
float Distribution::Square(int x, int y, int distance) {
	float sum = 0;
	int denominator = 0;
	if (y > 0) {
		sum += m_points[x][y - distance];
		denominator++;
	}
	if (y < m_width) {
		sum += m_points[x][y + distance];
		denominator++;
	}
	if (x > 0) {
		sum += m_points[x - distance][y];
		denominator++;
	}
	if (x < m_width) {
		sum += m_points[x + distance][y];
		denominator++;
	}
	return sum / denominator;
}
float Distribution::Deviation(float range) {
	return randWithin(-range / 2, range / 2);
}
float Distribution::BiomeDeviation(float zValue) {
	return Gaussian(zValue,1.f,0.f,4.f) * Sigmoid(zValue);
}
float Distribution::Gaussian(float x, float a, float b, float c) {
	return a * pow(M_E, -(((x - b)*(x - b)) / (2 * c*c)));
}
float Distribution::Sigmoid(float x, float a, float b, float c) {
	return a / (1 + pow(100, -((x - b) / c)));
}
void Distribution::Erosion() {
	const float depositK = 0.75;
	const float erodeK = 0.125;
	const float frictionK = 0.125;
	struct Water {
		Vector3 velocity;
		float volume;
		// sums up the change in volume for this cell
		float deltaV;
		float soil;
		float deltaS;
		Water() {
			velocity = Vector3(0.f, 0.f, 0.f);
			volume = 1.f;
			deltaV = 0.f;
			soil = 0.f;
			deltaS = 0.f;
		}
	};
	// create the water map
	vector< vector<Water> > waterMap = vector< vector<Water> >(m_width + 1, vector<Water>(m_height + 1));
	// update
	for (int iteration = 0; iteration < 4; iteration++) {
		for (int x = 0; x <= m_width; x++) {
			for (int y = 0; y <= m_width; y++) {
				// update cell
				waterMap[x][y].volume += waterMap[x][y].deltaV;
				waterMap[x][y].deltaV = 0;
				waterMap[x][y].soil += waterMap[x][y].deltaS;
				waterMap[x][y].deltaS = 0;
				// loss of energy
				waterMap[x][y].velocity *= frictionK;
				// update terrain
				if (waterMap[x][y].volume > 0) {
					float soil = 0.f;
					Vector3 vel(sqrt((2 * waterMap[x][y].velocity.x) / waterMap[x][y].volume), 0.f, sqrt((2 * waterMap[x][y].velocity.z) / waterMap[x][y].volume));
					float velocity = vel.Length();
					// deposit
					soil = waterMap[x][y].soil * depositK;
					// erode
					soil = -std::min(velocity * waterMap[x][y].volume * erodeK,3.f);

					m_points[x][y] += soil;
					waterMap[x][y].soil -= soil;
				}
				// friction
				waterMap[x][y].velocity.x *= 0.5;
				waterMap[x][y].velocity.z *= 0.5;
				float deltaV = 0.f;
				float soil = 0.f;
				// add valid ajacent cells to calculation
				for (int rot = 0; rot < 4; rot++) {
					int adjX = x + round(cos(rot * XM_PI / 2));
					int adjY = y + round(sin(rot * XM_PI / 2));
					if (adjX >= 0 && adjX <= m_width && adjY >= 0 && adjY <= m_width) {
						float difference = (m_points[adjX][adjY] + waterMap[adjX][adjY].volume) - (m_points[x][y] + waterMap[x][y].volume);
						float volume = std::max(-waterMap[x][y].volume / 4, std::min(difference / 4, waterMap[adjX][adjY].volume / 4));
						deltaV += volume;
						// velocity
						if (adjY == y) {
							waterMap[x][y].velocity.x += volume * 9.8 * difference;
						}
						if (adjX == x) {
							waterMap[x][y].velocity.z += volume * 9.8 * difference;
						}
						// soil transfer
						soil += std::max(-waterMap[x][y].soil / 4, std::min(difference / 4, waterMap[adjX][adjY].soil / 4));
					}
				}
				// set final delta values
				waterMap[x][y].deltaV += deltaV;
				waterMap[x][y].deltaS += soil;
			}
		}
	}
}
float Distribution::minimum_distance(Vector2 v, Vector2 w, Vector2 p) {
	// Return minimum distance between line segment vw and point p
	const float l2 = pow(abs(w.x-v.x) + abs(w.y - v.y),2);  // i.e. |w-v|^2 -  avoid a sqrt
	// We clamp t from [0,1] to handle points outside the segment vw.
	float t = (p-v).Dot(w - v) / l2 ;
	t = t > 1 ? 1 : t < 0 ? 0 : t;
	const Vector2 projection = v + t * (w - v);  // Projection falls on the segment
	return p.Distance(p,projection);
}
vector< vector<float> > Distribution::GetPoints() {
	return m_points;
}

float Distribution::CubicInterpolate(float p[4], float x) {
	return p[1] + 0.5 * x*(p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0])));
}

float Distribution::BicubicInterpolate(float p[4][4], float x, float y) {
	float arr[4];
	arr[0] = CubicInterpolate(p[0], y);
	arr[1] = CubicInterpolate(p[1], y);
	arr[2] = CubicInterpolate(p[2], y);
	arr[3] = CubicInterpolate(p[3], y);
	return CubicInterpolate(arr, x);
}
unsigned int Distribution::GetWidth() {
	return m_width;
}
Distribution::~Distribution() {

}
