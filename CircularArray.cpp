#include "pch.h"
#include "CircularArray.h"

CircularArray::CircularArray() {
}
void CircularArray::init(int widthX, int heightY) {
	m_width = widthX;
	m_height = heightY;
	m_size = widthX*heightY;
	/*for (int i = 0; i < m_size; i ++) {
		data
	}*/
	data = new Region[m_size];
}
void CircularArray::zeroOffsets() {
	xOffset = 0;
	yOffset = 0;
}
// managing the array
Region * CircularArray::get(int x, int y) {
	return &data[getIndex(x, y)];
}
void CircularArray::set(int x, int y, ID3D11Device * device, int regX, int regZ, unsigned int worldWidthIn, unsigned int regionWidthIn, string workingPathIn) {
	data[getIndex(x,y)].init(device,regX,regZ,worldWidthIn,regionWidthIn,workingPathIn);
}
// get properties
int CircularArray::getWidth() {
	return m_width;
}
int CircularArray::getHeight() {
	return m_height;
}
int CircularArray::size() {
	return m_size;
}
int CircularArray::getIndex(int x, int y) {
	// calculates x and y position based off of offsets
	return mod((x + xOffset), m_width) + (mod((y + yOffset), m_height)*m_width);
}
// shift methods
void CircularArray::offsetX(int offset) {
	xOffset = mod((xOffset + offset), m_width);
}
void CircularArray::offsetY(int offset) {
	yOffset = mod((yOffset + offset), m_height);
}
CircularArray::~CircularArray() {
	//delete[] data;
}
int CircularArray::mod(int k, int n) {
	return ((k %= n) < 0) ? k + n : k;
}
