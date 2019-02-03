#pragma once
#include "AssetTypes.h"
typedef uint32_t AssetID;
typedef uint8_t TransformID;
enum Transforms {
	None,
	Rotate90,
	Rotate180,
	Rotate270,
	TranslateQ2,
	TranslateQ3,
	TranslateQ4
};
static const Matrix TRANSFORMS[]{
	Matrix::CreateRotationY(0.f),
	Matrix::CreateRotationY(XM_PIDIV2),
	Matrix::CreateRotationY(XM_PI),
	Matrix::CreateRotationY(XM_PI + XM_PIDIV2),
	Matrix::CreateTranslation(0.f,0.f,-0.5f),
	Matrix::CreateTranslation(-0.5f,0.f,-0.5f),
	Matrix::CreateTranslation(-0.5f,0.f,0.f)
};