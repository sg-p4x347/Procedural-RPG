#pragma once

#include <vector>

using namespace std;

template <class dataType> 
class CircularArray{
public:
	// constructors
	CircularArray();
	CircularArray(int widthX, int heightY);
	~CircularArray();
	// managing the array
	dataType get(int x, int y);
	void set(int x, int y, dataType input);
	// get properties
	int getWidth();
	int getHeight();
	int size();
	// shift methods, they shift the point of view relative to where it is located
	void offsetX(int offset);
	void offsetY(int offset);
	void zeroOffsets();

private:
	int getIndex(int x, int y);
	dataType * data;
	int xOffset = 0;
	int yOffset = 0;
	// properties
	int m_width;
	int m_height;
	int m_size;

};
template <class dataType>
CircularArray<dataType>::CircularArray() {
}
template <class dataType>
CircularArray<dataType>::CircularArray(int widthX, int heightY) {
	m_width = widthX;
	m_height = heightY;
	m_size = widthX*heightY;
	data = new dataType[m_size];
}
template <class dataType>
void CircularArray<dataType>::zeroOffsets() {
	xOffset = 0;
	yOffset = 0;
}
// managing the array
template <class dataType>
dataType CircularArray<dataType>::get(int x, int y) {
	return data[getIndex(abs((x + xOffset) % m_width), abs((y + yOffset) % m_height))];
}
template <class dataType>
void CircularArray<dataType>::set(int x, int y, dataType input) {
	data[getIndex(abs((x + xOffset) % m_width), abs((y + yOffset) % m_height))] = input;
}
// get properties
template <class dataType>
int CircularArray<dataType>::getWidth() {
	return m_width;
}
template <class dataType>
int CircularArray<dataType>::getHeight() {
	return m_height;
}
template <class dataType>
int CircularArray<dataType>::size() {
	return m_size;
}
template <class dataType>
int CircularArray<dataType>::getIndex(int x, int y) {
	return x + (y*m_width);
}
// shift methods
template <class dataType>
void CircularArray<dataType>::offsetX(int offset) {
	xOffset = abs((xOffset + offset) % m_width);
}
template <class dataType>
void CircularArray<dataType>::offsetY(int offset) {
	yOffset = abs((yOffset + offset) % m_height);
}
template <class dataType>
CircularArray<dataType>::~CircularArray() {
	delete[] data;
}
