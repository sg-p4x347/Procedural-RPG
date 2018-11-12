#include "pch.h"
#include "CollisionAsset.h"
#include "JsonUtil.h"

CollisionAsset::CollisionAsset() : Components::Component()
{
}


CollisionAsset::~CollisionAsset()
{
}

string CollisionAsset::GetName()
{
	return "CollisionAsset";
}

void CollisionAsset::Import(std::istream & ifs)
{
	Import(JsonParser(ifs));
}

void CollisionAsset::Export(std::ostream & ofs)
{
	Export().Export(ofs);
}

void CollisionAsset::Import(JsonParser & json)
{
	if (json.IsDefined("BoundingSpheres")) {
		for (auto & sphereData : json["BoundingSpheres"].ToVector<JsonParser>()) {
			BoundingSphere sphere;
			sphere.Center = JsonToVector3(sphereData["Center"]);
			sphere.Radius = sphereData["Radius"].To<float>();
			BoundingSpheres.push_back(sphere);
		}
	}
	if (json.IsDefined("AxisAlignedBoxes")) {
		for (auto & boxData : json["AxisAlignedBoxes"].ToVector<JsonParser>()) {
			BoundingBox box;
			box.Center = JsonToVector3(boxData["Center"]);
			box.Extents = JsonToVector3(boxData["Extents"]);
			AxisAlignedBoxes.push_back(box);
		}
	}
	if (json.IsDefined("OrientedBoxes")) {
		for (auto & boxData : json["OrientedBoxes"].ToVector<JsonParser>()) {
			BoundingOrientedBox box;
			box.Center = JsonToVector3(boxData["Center"]);
			box.Extents = JsonToVector3(boxData["Extents"]);
			box.Orientation = JsonToVector4(boxData["Orientation"]);
			OrientedBoxes.push_back(box);
		}
	}
}

JsonParser CollisionAsset::Export()
{
	JsonParser jp;
	if (BoundingSpheres.size()) {
		JsonParser spheres(JsonType::array);
		for (auto & sphere : BoundingSpheres) {
			JsonParser sphereData(JsonType::object);
			sphereData.Set("Center", Vector3ToJson(sphere.Center));
			sphereData.Set("Radius", sphere.Radius);
			spheres.Add(sphereData);
		}
		jp.Set("BoundingSpheres", spheres);
	}
	if (AxisAlignedBoxes.size()) {
		JsonParser boxes(JsonType::array);
		for (auto & box : AxisAlignedBoxes) {
			JsonParser boxData(JsonType::object);
			boxData.Set("Center", Vector3ToJson(box.Center));
			boxData.Set("Extents", Vector3ToJson(box.Extents));
			boxes.Add(boxData);
		}
		jp.Set("AxisAlignedBoxes", boxes);
	}
	if (OrientedBoxes.size()) {
		JsonParser boxes(JsonType::array);
		for (auto & box : OrientedBoxes) {
			JsonParser boxData(JsonType::object);
			boxData.Set("Center", Vector3ToJson(box.Center));
			boxData.Set("Extents", Vector3ToJson(box.Extents));
			boxData.Set("Orientation", Vector4ToJson(box.Orientation));
			boxes.Add(boxData);
		}
		jp.Set("OrientedBoxes", boxes);
	}
	return jp;
}
