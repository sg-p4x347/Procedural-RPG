#include "Distribution.h"

// Diamond Square distribution algorithm
Distribution::Distribution (float maxDeviation, const float deviationDecrease,const int zoom,const bool isMountain) {
	input_maxDeviation = maxDeviation;
	input_deviationDecrease = deviationDecrease;
	input_zoom = zoom;
	input_isMountain = isMountain;
	// initialize the corners
	float corners = isMountain ? -input_maxDeviation : 0.0;
	points[0][0] = corners;
	points[0][mapWidth] = corners;
	points[mapWidth][0] = corners;
	points[mapWidth][mapWidth] = corners;
	// iterate
	for (int iteration = 0; iteration < floor(log2(mapWidth)); iteration++) {
		int gridWidth = (mapWidth / pow(2, iteration))/2;
		for (int x = gridWidth; x < mapWidth; x += gridWidth*2) {
			for (int y = gridWidth; y < mapWidth; y += gridWidth*2) {
				// Diamond
				points[x][y] = isMountain && iteration == 0 ? input_maxDeviation*4 : diamond(x, y, gridWidth) + deviation(maxDeviation);
				// Square
				if (x > 0 && x < mapWidth && y - gridWidth > 0 && y - gridWidth < mapWidth) {
					points[x][y - gridWidth] = square(x, y - gridWidth, gridWidth) + deviation(maxDeviation);
				} else {
					points[x][y - gridWidth] = isMountain ? -input_maxDeviation : square(x, y - gridWidth, gridWidth) + deviation(maxDeviation);
				}
				if (x - gridWidth > 0 && x - gridWidth < mapWidth && y > 0 && y < mapWidth) {
					points[x - gridWidth][y] = square(x - gridWidth, y, gridWidth) + deviation(maxDeviation);
				} else {
					points[x - gridWidth][y] = isMountain ? -input_maxDeviation : square(x - gridWidth, y, gridWidth) + deviation(maxDeviation);
				}
				if (x > 0 && x < mapWidth && y + gridWidth > 0 && y + gridWidth < mapWidth) {
					points[x][y + gridWidth] = square(x, y + gridWidth, gridWidth) + deviation(maxDeviation);
				} else {
					points[x][y + gridWidth] = isMountain ? -input_maxDeviation : square(x, y + gridWidth, gridWidth) + deviation(maxDeviation);
				}
				if (x + gridWidth > 0 && x + gridWidth < mapWidth && y > 0 && y < mapWidth) {
					points[x + gridWidth][y] = square(x + gridWidth, y, gridWidth) + deviation(maxDeviation);
				} else {
					points[x + gridWidth][y] = isMountain ? -input_maxDeviation : square(x + gridWidth, y, gridWidth) + deviation(maxDeviation);
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
Distribution::Distribution(float maxDeviation, const float deviationDecrease, const int zoom, Distribution * continentMap, Distribution * biomeMap) {
	input_maxDeviation = maxDeviation;
	input_deviationDecrease = deviationDecrease;
	input_zoom = zoom;
	// initialize the corners
	points[0][0] = continentMap->points[0][0];
	points[0][mapWidth] = continentMap->points[0][mapWidth];
	points[mapWidth][0] = continentMap->points[mapWidth][0];;
	points[mapWidth][mapWidth] = continentMap->points[mapWidth][mapWidth];
	// iterate
	for (int iteration = 0; iteration < floor(log2(mapWidth)); iteration++) {
		int gridWidth = (mapWidth / pow(2, iteration)) / 2;
		for (int x = gridWidth; x < mapWidth; x += gridWidth * 2) {
			for (int y = gridWidth; y < mapWidth; y += gridWidth * 2) {
				float flatConstant = 0.2;
				float biomeConstant = 0.2;

				// NOTE --------------------------------------------------------------------------------------------------------
				/* zoom level in continent generation refers to the iteration at which control points will stop being taken from 
				the continent and biome map, aftwer which, the algorithm reverts to the regular diamond-square algorithm of
				"average and add random value". The random deviation maximum is determined by the biome map, along with the 
				continent's default roughness*/
				// NOTE --------------------------------------------------------------------------------------------------------

				// Diamond
				if (iteration <= zoom) {
					if (continentMap->points[x][y] > 0) {
						points[x][y] = continentMap->points[x][y] * flatConstant + biomeDeviation(biomeMap->points[x][y], iteration, zoom) + deviation(biomeDeviation(biomeMap->points[x][y], iteration, zoom)*biomeConstant) + deviation(maxDeviation);
					} else {
						points[x][y] = continentMap->points[x][y] + biomeDeviation(biomeMap->points[x][y], iteration, zoom) + deviation(biomeDeviation(biomeMap->points[x][y], iteration, zoom)*biomeConstant) + deviation(maxDeviation);
					}
				} else {
					points[x][y] = diamond(x, y, gridWidth) + deviation(biomeDeviation(biomeMap->points[x][y], iteration, zoom)*biomeConstant) + deviation(maxDeviation);
				}
				// Square
				if (iteration <= zoom) {
					if (x > 0 && x < mapWidth && y - gridWidth > 0 && y - gridWidth < mapWidth) {
						points[x][y - gridWidth] = continentMap->points[x][y - gridWidth] > 0 ?
							continentMap->points[x][y - gridWidth] * flatConstant + biomeDeviation(biomeMap->points[x][y - gridWidth], iteration, zoom) + deviation(biomeDeviation(biomeMap->points[x][y - gridWidth], iteration, zoom)*biomeConstant) + deviation(maxDeviation) : continentMap->points[x][y - gridWidth] + biomeDeviation(biomeMap->points[x][y - gridWidth], iteration, zoom) + deviation(biomeDeviation(biomeMap->points[x][y - gridWidth], iteration, zoom)*biomeConstant) + deviation(maxDeviation);
					} else {
						points[x][y - gridWidth] = continentMap->points[x][y - gridWidth];
					}
					if (x - gridWidth > 0 && x - gridWidth < mapWidth && y > 0 && y < mapWidth) {
						points[x - gridWidth][y] = continentMap->points[x - gridWidth][y] > 0 ?
							continentMap->points[x - gridWidth][y] * flatConstant + biomeDeviation(biomeMap->points[x - gridWidth][y], iteration, zoom) + deviation(biomeDeviation(biomeMap->points[x - gridWidth][y], iteration, zoom)*biomeConstant) + deviation(maxDeviation) : continentMap->points[x - gridWidth][y] + biomeDeviation(biomeMap->points[x - gridWidth][y], iteration, zoom) + deviation(biomeDeviation(biomeMap->points[x - gridWidth][y], iteration, zoom)*biomeConstant) + deviation(maxDeviation);
					} else {
						points[x - gridWidth][y] = continentMap->points[x - gridWidth][y];
					}
					if (x > 0 && x < mapWidth && y + gridWidth > 0 && y + gridWidth < mapWidth) {
						points[x][y + gridWidth] = continentMap->points[x][y + gridWidth] > 0 ?
							continentMap->points[x][y + gridWidth] * flatConstant + biomeDeviation(biomeMap->points[x][y + gridWidth], iteration, zoom) + deviation(biomeDeviation(biomeMap->points[x][y + gridWidth], iteration, zoom)*biomeConstant) + deviation(maxDeviation) : continentMap->points[x][y + gridWidth] + biomeDeviation(biomeMap->points[x][y + gridWidth], iteration, zoom) + deviation(biomeDeviation(biomeMap->points[x][y + gridWidth], iteration, zoom)*biomeConstant) + deviation(maxDeviation);
					} else {
						points[x][y + gridWidth] = continentMap->points[x][y + gridWidth];
					}
					if (x + gridWidth > 0 && x + gridWidth < mapWidth && y > 0 && y < mapWidth) {
						points[x + gridWidth][y] = continentMap->points[x + gridWidth][y] > 0 ?
							continentMap->points[x + gridWidth][y] * flatConstant + biomeDeviation(biomeMap->points[x + gridWidth][y], iteration, zoom) + deviation(biomeDeviation(biomeMap->points[x + gridWidth][y], iteration, zoom)*biomeConstant) + deviation(maxDeviation) : continentMap->points[x + gridWidth][y] + biomeDeviation(biomeMap->points[x + gridWidth][y], iteration, zoom) + deviation(biomeDeviation(biomeMap->points[x + gridWidth][y], iteration, zoom)*biomeConstant) + deviation(maxDeviation);
					} else {
						points[x + gridWidth][y] = continentMap->points[x + gridWidth][y];
					}
				} else {
					if (x > 0 && x < mapWidth && y - gridWidth > 0 && y - gridWidth < mapWidth) {
						points[x][y - gridWidth] = true || continentMap->points[x][y - gridWidth] > 0 ? square(x, y - gridWidth, gridWidth) + deviation(biomeDeviation(biomeMap->points[x][y - gridWidth], iteration, zoom)*biomeConstant) + deviation(maxDeviation) : continentMap->points[x][y - gridWidth] + biomeDeviation(biomeMap->points[x][y - gridWidth], iteration, zoom) + deviation(biomeDeviation(biomeMap->points[x][y - gridWidth], iteration, zoom)*biomeConstant) + deviation(maxDeviation);
					} else {
						points[x][y - gridWidth] = continentMap->points[x][y - gridWidth];
					}
					if (x - gridWidth > 0 && x - gridWidth < mapWidth && y > 0 && y < mapWidth) {
						points[x - gridWidth][y] = true || continentMap->points[x - gridWidth][y] > 0 ? square(x - gridWidth, y, gridWidth) + deviation(biomeDeviation(biomeMap->points[x - gridWidth][y], iteration, zoom)*biomeConstant) + deviation(maxDeviation) : continentMap->points[x - gridWidth][y] + biomeDeviation(biomeMap->points[x - gridWidth][y], iteration, zoom) + deviation(biomeDeviation(biomeMap->points[x - gridWidth][y], iteration, zoom)*biomeConstant) + deviation(maxDeviation);
					} else {
						points[x - gridWidth][y] = continentMap->points[x - gridWidth][y];
					}
					if (x > 0 && x < mapWidth && y + gridWidth > 0 && y + gridWidth < mapWidth) {
						points[x][y + gridWidth] = true || continentMap->points[x][y + gridWidth] > 0 ? square(x, y + gridWidth, gridWidth) + deviation(biomeDeviation(biomeMap->points[x][y + gridWidth], iteration, zoom)*biomeConstant) + deviation(maxDeviation) : continentMap->points[x][y + gridWidth] + biomeDeviation(biomeMap->points[x][y + gridWidth], iteration, zoom) + deviation(biomeDeviation(biomeMap->points[x][y + gridWidth], iteration, zoom)*biomeConstant) + deviation(maxDeviation);
					} else {
						points[x][y + gridWidth] = continentMap->points[x][y + gridWidth];
					}
					if (x + gridWidth > 0 && x + gridWidth < mapWidth && y > 0 && y < mapWidth) {
						points[x + gridWidth][y] = true || continentMap->points[x + gridWidth][y] > 0 ? square(x + gridWidth, y, gridWidth) + deviation(biomeDeviation(biomeMap->points[x + gridWidth][y], iteration, zoom)*biomeConstant) + deviation(maxDeviation) : continentMap->points[x + gridWidth][y] + biomeDeviation(biomeMap->points[x + gridWidth][y], iteration, zoom) + deviation(biomeDeviation(biomeMap->points[x + gridWidth][y], iteration, zoom)*biomeConstant) + deviation(maxDeviation);
					} else {
						points[x + gridWidth][y] = continentMap->points[x + gridWidth][y];
					}
				}
			}
		}
		// decrease the random deviation range
		maxDeviation -= deviationDecrease * maxDeviation;
	}
}
// Diamond
float Distribution::diamond(int x, int y, int distance) {
	float sum = 0;
	sum += points[x - distance][y - distance];
	sum += points[x - distance][y + distance];
	sum += points[x + distance][y - distance];
	sum += points[x + distance][y + distance];
	return sum / 4;
}
// Square
float Distribution::square(int x, int y, int distance) {
	float sum = 0;
	int denominator = 0;
	if (y > 0) {
		sum += points[x][y - distance];
		denominator++;
	}
	if (y < mapWidth) {
		sum += points[x][y + distance];
		denominator++;
	}
	if (x > 0) {
		sum += points[x - distance][y];
		denominator++;
	}
	if (x < mapWidth) {
		sum += points[x + distance][y];
		denominator++;
	}
	return sum / denominator;
}
float Distribution::deviation(float range) {
	return randWithin(-range / 2, range / 2);
}
float Distribution::biomeDeviation(float zValue, int iteration, int zoom) {
	/* Gaussuian curve function
	a*e^-(((x+b)^2)/(2c^2))
	*/
	// width of the bell curve
	float width = 4;
	// height of the bell curve
	float height = 8;
	float deviationDecrease = pow(M_E, -((zValue)*(zValue)) / (2 * width*width));
	float initialDeviation = height * deviationDecrease;
	float maxDeviation = initialDeviation*pow(deviationDecrease, iteration - zoom);
	if (maxDeviation > initialDeviation) {
		maxDeviation = initialDeviation;
	}
	return maxDeviation;
}
int Distribution::getWidth() {
	return mapWidth;
}
Distribution::~Distribution() {

}
