#pragma once
#include "Vector3.hpp"

#undef min
#undef max

/*
	Bounds

	AABB/Sphere
*/

struct Bounds
{
	//AABB
	Vector3 min;
	Vector3 max;

	//Sphere
	Vector3 centre;
	float radius;

	Bounds(const Vector3& extent, const Vector3& centre = Vector3()) :
		centre(centre), 
		radius(0.f), 
		min(centre - extent), max(centre + extent) 
	{ RecalculateSphereBounds(); }

	Bounds(const Vector3& centre, float radius) : 
		centre(centre), 
		radius(radius), 
		min(centre.x - radius, centre.y - radius, centre.z - radius), max(centre.x + radius, centre.y + radius, centre.z + radius) 
	{}

	Bounds() : min(), max(), centre(), radius(0.f) {}

	static Bounds FromMinMax(const Vector3& min, const Vector3& max)
	{
		return Bounds((max - min) / 2.f, (max + min) / 2.f);
	}

	void RecalculateSphereBounds();

	Vector3 GetExtent() const { return (max - min) / 2.f; }
};
