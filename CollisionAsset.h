#pragma once
#include "Component.h"
#include "JSON.h"
class CollisionAsset :
	public Components::Component,
	public JSON
{
public:
	CollisionAsset();
	~CollisionAsset();

	// Inherited via Component
	virtual string GetName() override;
	virtual void Import(std::istream & ifs) override;
	virtual void Export(std::ostream & ofs) override;
	// Inherited via JSON
	virtual void Import(JsonParser & json) override;
	virtual JsonParser Export() override;

	std::vector<BoundingSphere> BoundingSpheres;
	std::vector<BoundingBox> AxisAlignedBoxes;
	std::vector<BoundingOrientedBox> OrientedBoxes;
};

