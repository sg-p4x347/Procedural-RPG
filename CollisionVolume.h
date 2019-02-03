#pragma once
#include "ISerialization.h"
/*
Base class for specific collision volumes
*/
namespace geometry {
	class CollisionVolume :
		public ISerialization
	{
	public:
		CollisionVolume();
		~CollisionVolume();
		// Returns the furthest point within the volume in the given direction
		virtual Vector3 Support(Vector3 direction) = 0;
		// Returns the furthest points (backwards,forwards) in the given direction
		virtual std::pair<Vector3, Vector3> BiSupport(Vector3 direction);
		// Returns a new collision volume instance transformed by the given matrix
		virtual shared_ptr<CollisionVolume> Transform(DirectX::SimpleMath::Matrix matrix) = 0;
		// Returns an axis aligned bounding box that contains the volume
		virtual BoundingBox Bounds();
		// Returns the min and max dot products in the given direction
		virtual std::pair<float, float> Project(Vector3 direction);
		// Returns a set of normal vectors
		virtual std::vector<Vector3> Normals() = 0;
	};
}
