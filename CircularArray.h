#pragma once

#include "Region.h"

class CircularArray{
public:
	// constructors
	CircularArray();
	void init(int widthX, int heightY);
	~CircularArray();
	// managing the array
	Region* get(int x, int y);
	void set(int x, int y, shared_ptr<ID3D11Device> device, int regX, int regZ,unsigned int worldWidthIn, unsigned int regionWidthIn, string workingPathIn);
	// get properties
	int getWidth();
	int getHeight();
	int size();
	// shift methods, they shift the point of view relative to where it is located
	void offsetX(int offset);
	void offsetY(int offset);
	void zeroOffsets();
	Region * data;
private:
	
	int getIndex(int x, int y);
	int xOffset = 0;
	int yOffset = 0;
	// properties
	int m_width;
	int m_height;
	int m_size;

};

