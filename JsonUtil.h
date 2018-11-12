#pragma once
#include "pch.h"
#include "JSON.h"


JsonParser Vector3ToJson(Vector3  vec) {
	JsonParser jp(JsonType::object);
	jp.Set("x", vec.x);
	jp.Set("y", vec.y);
	jp.Set("z", vec.z);
	return jp;
}
Vector3 JsonToVector3(JsonParser & json) {
	return Vector3(json["x"].To<float>(), json["y"].To<float>(), json["z"].To<float>());
}
JsonParser Vector4ToJson(Vector4  vec) {
	JsonParser jp(JsonType::object);
	jp.Set("x", vec.x);
	jp.Set("y", vec.y);
	jp.Set("z", vec.z);
	jp.Set("w", vec.w);
	return jp;
}
Vector4 JsonToVector4(JsonParser & json) {
	return Vector4(json["x"].To<float>(), json["y"].To<float>(), json["z"].To<float>(),json["w"].To<float>());
}