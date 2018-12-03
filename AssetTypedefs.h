#pragma once

typedef uint32_t AssetID;
typedef uint8_t TransformID;
enum Transforms {
	None,
	Rotate90,
	Rotate180,
	Rotate270
};
static const Matrix TRANSFORMS[]{
	Matrix::CreateRotationY(0.f),
	Matrix::CreateRotationY(XM_PIDIV2),
	Matrix::CreateRotationY(XM_PI),
	Matrix::CreateRotationY(XM_PI + XM_PIDIV2),
};