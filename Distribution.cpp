#include "pch.h"
#include "Distribution.h"
#include "ConfigParser.h"
#include <chrono>
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
	m_points[0][0] = isMountain ? -m_maxDeviation : Deviation(maxDeviation);
	m_points[0][m_width] = isMountain ? -m_maxDeviation : Deviation(maxDeviation);
	m_points[m_width][0] = isMountain ? -m_maxDeviation : Deviation(maxDeviation);
	m_points[m_width][m_width] = isMountain ? -m_maxDeviation : Deviation(maxDeviation);
	// iterate
	int finalIteration = floor(log2(m_width));
	for (int iteration = 0; iteration < finalIteration; iteration++) {
		int gridWidth = (m_width / pow(2, iteration)) * 0.5;
		for (int x = gridWidth; x < m_width; x += gridWidth * 2) {
			for (int y = gridWidth; y < m_width; y += gridWidth * 2) {
				// Diamond
				m_points[x][y] = isMountain && iteration == 0 ? m_maxDeviation : Diamond(x, y, gridWidth) + Deviation(maxDeviation);
				// Square
				for (float rad = (x == m_width - gridWidth || y == m_width - gridWidth ? 0.f : 2.f); rad < 4.f; rad++) {
					int pointX = round(x + cos(rad * XM_PI * 0.5f) * gridWidth);
					int pointY = round(y + sin(rad * XM_PI * 0.5f) * gridWidth);
					if (isMountain && (pointX == 0 || pointX == m_width || pointY == 0 || pointY == m_width)) {
						// point is on the edge of the mountain
						m_points[pointX][pointY] = -m_maxDeviation; 
					} else {
						// use the square method to calculate the elevation
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
// Diamond
float Distribution::Diamond(int x, int y, int distance) {
	float sum = 0.f;
	sum += m_points[x - distance][y - distance];
	sum += m_points[x - distance][y + distance];
	sum += m_points[x + distance][y - distance];
	sum += m_points[x + distance][y + distance];
	return sum * 0.25f;
}
// Square
// Two of the square calculations are unecessary, as they get overridden by the adjacent pass
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
// continent generator
void Distribution::Continent(int ctrlWidth) {
	ConfigParser config("config/continent.cfg");
	// create the base continent map

	unique_ptr<Distribution> continentDist(new Distribution(m_width, m_height));
	auto start = std::chrono::high_resolution_clock::now();
	continentDist->DiamondSquare(m_width * config.GetDouble("CM_initialDeviation"), config.GetDouble("CM_roughness"), config.GetInt("CM_zoom"), true);
	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	
	vector< vector<float> > continentMap = continentDist->GetPoints();
	// create the biome distribution map
	unique_ptr<Distribution> biomeDist(new Distribution(m_width, m_height));
	biomeDist->DiamondSquare(config.GetDouble("BM_initialDeviation"), config.GetDouble("BM_roughness"), config.GetInt("BM_zoom"), false);
	vector< vector<float> > biomeMap = biomeDist->GetPoints();

	m_maxDeviation = m_width * config.GetDouble("TM_initialDeviation");
	m_deviationDecrease = config.GetDouble("TM_roughness");
	float maxDeviation = m_maxDeviation;
	// amplitude
	const double landAmplitude = config.GetDouble("landAmplitude");
	const double oceanAmplitude = config.GetDouble("oceanAmplitude");
	const double biomeDeviationConst = config.GetDouble("biomeDeviationConst");
	const int biomeCutoff = config.GetInt("biomeCutoff");
	const double offset = m_maxDeviation * config.GetDouble("offset");
	// initialize the corners
	for (int x = 0; x <= m_width; x += m_width) {
		for (int y = 0; y <= m_width; y += m_width) {
			float continentZ = continentMap[x][y] + offset;
			m_points[x][y] = continentZ * oceanAmplitude + BiomeDeviation(biomeMap[x][y], continentZ) + (Deviation(BiomeDeviation(biomeMap[x][y], continentZ)));
		}
	}
	// iterate
	for (int iteration = 0; iteration < floor(log2(m_width)); iteration++) {
		int gridWidth = (m_width / pow(2, iteration)) / 2;
		for (int x = gridWidth; x < m_width; x += gridWidth * 2) {
			for (int y = gridWidth; y < m_width; y += gridWidth * 2) {
				// Diamond
				float points[4][4];
				if (gridWidth >= ctrlWidth) {
					float continentZ = continentMap[x][y] + offset;
					if (continentZ > 0) {
						continentZ *= landAmplitude;
					} else {
						continentZ *= oceanAmplitude;
					}
					if (gridWidth >= biomeCutoff) {
						m_points[x][y] = continentZ;
						m_points[x][y] += BiomeDeviation(biomeMap[x][y], continentZ) + (Deviation(BiomeDeviation(biomeMap[x][y], continentZ)));
					} else {
						m_points[x][y] = Diamond(x, y, gridWidth) + Deviation(maxDeviation) + (Deviation(maxDeviation) * Deviation(BiomeDeviation(biomeMap[x][y], continentZ))*biomeDeviationConst);
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
						float continentZ = continentMap[pointX][pointY] + offset;
						if (continentZ > 0) {
							continentZ *= landAmplitude;
						}
						else {
							continentZ *= oceanAmplitude;
						}
						if (gridWidth >= biomeCutoff) {
							m_points[pointX][pointY] = continentZ;
							m_points[pointX][pointY] += BiomeDeviation(biomeMap[pointX][pointY], continentZ) + (Deviation(BiomeDeviation(biomeMap[pointX][pointY], continentZ)));
						} else {
							m_points[pointX][pointY] = Square(pointX, pointY, gridWidth) + Deviation(maxDeviation) + (Deviation(maxDeviation) * Deviation(BiomeDeviation(biomeMap[pointX][pointY], continentZ))*biomeDeviationConst);
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
float Distribution::Deviation(float range) {
	return randWithin(-range / 2, range / 2);
}
float Distribution::BiomeDeviation(float biome, float continent) {
	return Gaussian(biome,16.f,0.f,0.5) * Sigmoid(continent,1.f,0.f,4.f);
}
// a controls amplituide
// b controls x displacement
// c controls width
float Distribution::Gaussian(float x, float a, float b, float c) {
	return a * pow(100, -pow((x - b)/c,2));
}
float Distribution::Sigmoid(float x, float a, float b, float c) {
	return a / (1 + pow(100, -((x - b) / c)));
}
void Distribution::Erosion() {
	const float depositK = 0.25f;
	const float erodeK = 1.f;
	const float frictionK = 0.05;
	struct Water {
		Vector3 KE;
		Vector3 deltaKE;
		float water;
		float deltaW;
		float soil;
		float deltaS;
		float lowestNeighbor;
		
		Water() {
			KE = Vector3(0.f, 0.f, 0.f);
			deltaKE = Vector3(0.f, 0.f, 0.f);
			water = 1.f;
			deltaW = 0.f;
			soil = 0.f;
			deltaS = 0.f;
		}
	};
	// create the water map
	vector< vector<Water> > waterMap = vector< vector<Water> >(m_width + 1, vector<Water>(m_height + 1));
	// update
	for (int iteration = 0; iteration < 5; iteration++) {
		for (int x = 0; x <= m_width; x++) {
			for (int y = 0; y <= m_width; y++) {
				float volume = waterMap[x][y].water + waterMap[x][y].soil;
				float deltaW = 0.f;
				float deltaS = 0.f;
				// add valid ajacent cells to calculation
				waterMap[x][y].lowestNeighbor = m_points[x][y];
				for (int rot = 0; rot < 4; rot++) {
					int adjX = x + round(cos(rot * XM_PI / 2));
					int adjY = y + round(sin(rot * XM_PI / 2));
					if (adjX >= 0 && adjX <= m_width && adjY >= 0 && adjY <= m_width) {
						float adjVolume = waterMap[adjX][adjY].water + waterMap[adjX][adjY].soil;
						if (m_points[adjX][adjY] < waterMap[x][y].lowestNeighbor) {
							waterMap[x][y].lowestNeighbor = m_points[adjX][adjY];
						}
						float difference = (m_points[adjX][adjY] + adjVolume) - (m_points[x][y] + volume);
						float transferVolume = std::max(-volume / 4, std::min(difference / 4, adjVolume / 4));
						// material transfer
						float overlap;
						if (transferVolume > 0) {
							deltaW += (waterMap[adjX][adjY].water / adjVolume) * transferVolume;
							deltaS += (waterMap[adjX][adjY].soil / adjVolume) * transferVolume;
							overlap = m_points[x][y] + volume - m_points[adjX][adjY];
						}
						else {
							deltaW += (waterMap[x][y].water / volume) * transferVolume;
							deltaS += (waterMap[x][y].soil / volume) * transferVolume;
							overlap = m_points[adjX][adjY] + adjVolume - m_points[x][y];
						}
						// soil diffusion
						if (overlap > 0) {
							deltaS += (((waterMap[adjX][adjY].soil / adjVolume) * overlap) + ((waterMap[x][y].soil / volume) * overlap)) / 2 - ((waterMap[x][y].soil / volume) * overlap);
						}
						// add gravitational and kinetic energy
						if (abs(transferVolume) > 0) {
							if (adjY == y) {
								waterMap[x][y].deltaKE.x += (transferVolume * 9.8 * difference) + (transferVolume > 0 ? waterMap[adjX][adjY].KE.x * (transferVolume / (waterMap[adjX][adjY].water + waterMap[adjX][adjY].soil)) : waterMap[x][y].KE.x * (transferVolume / (waterMap[x][y].water + waterMap[x][y].soil)));
							}
							if (adjX == x) {
								waterMap[x][y].deltaKE.z += (transferVolume * 9.8 * difference) + (transferVolume > 0 ? waterMap[adjX][adjY].KE.z * (transferVolume / (waterMap[adjX][adjY].water + waterMap[adjX][adjY].soil)) : waterMap[x][y].KE.z * (transferVolume / (waterMap[x][y].water + waterMap[x][y].soil)));
							}
						}
					}
				}
				// set final delta values
				waterMap[x][y].deltaW += deltaW;
				waterMap[x][y].deltaS += deltaS;
			}
		}
		for (int x = 0; x <= m_width; x++) {
			for (int y = 0; y <= m_width; y++) {
				// evaporation
				waterMap[x][y].water *= 0.75;
				// add surface water
				waterMap[x][y].water += 1;
				// update cell
				waterMap[x][y].KE += waterMap[x][y].deltaKE;
				waterMap[x][y].deltaKE = Vector3(0.f,0.f,0.f);
				waterMap[x][y].water += waterMap[x][y].deltaW;
				waterMap[x][y].deltaW = 0;
				waterMap[x][y].soil += waterMap[x][y].deltaS;
				waterMap[x][y].deltaS = 0;
				// loss of energy
				waterMap[x][y].KE *= frictionK;
				// update terrain
				if (waterMap[x][y].water > 0) {
					float volume = waterMap[x][y].water + waterMap[x][y].soil;
					float soil = 0.f;
					Vector3 velocity(sqrt((2 * waterMap[x][y].KE.x) / volume), 0.f, sqrt((2 * waterMap[x][y].KE.z) / volume));
					float speed = velocity.Length();
					
					// erode
					soil = Sigmoid(speed, (waterMap[x][y].lowestNeighbor - m_points[x][y]) * erodeK, 0.f,5.f );
					m_points[x][y] += soil;
					waterMap[x][y].soil -= soil;
					// deposit
					//if (velocity < 1.f && waterMap[x][y].soil > 3) {
						soil = (waterMap[x][y].soil * depositK) - Sigmoid(speed, waterMap[x][y].soil * depositK, 0.f, 5.f);
						m_points[x][y] += soil;
						waterMap[x][y].soil -= soil;
					//}
				}
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
