#pragma once
#include "ECollisionChannels.hpp"
#include "CollisionShape.hpp"
#include <ELCore/Buffer.hpp>
#include <ELCore/Pair.hpp>
#include <ELMaths/Vector3.hpp>

struct LineSegment;
struct Ray;
struct RaycastHitInformation;

class Transform;

enum class EOverlapResult
{
	SEPERATE = 0,
	TOUCHING = 1,
	OVERLAPPING = 2
};

class Collider
{
	ECollisionChannels _channels;
	Buffer<CollisionShape*> _shapes;

	float _maxRadius;

	void _OnShapeChanged();

public:
	Collider() : _channels(ECollisionChannels::NONE), _maxRadius(0.f) {}
	Collider(ECollisionChannels channels, const CollisionShape& shape);
	Collider(ECollisionChannels channels, const Buffer<CollisionShape>& shapes = Buffer<CollisionShape>());
	Collider(const Collider& other);
	Collider(Collider&& other) noexcept;
	~Collider();

	Collider& operator=(const Collider& other);
	Collider& operator=(Collider&& other) noexcept;

	const ECollisionChannels& GetChannels() const { return _channels; }
	float GetBoundingRadius() const { return _maxRadius; }

	template <typename T>
	requires Concepts::DerivedFrom<T, CollisionShape>
	T& AddShape(const T& shape)										
	{ 
		T& shape = *_shapes.Add(shape.Clone());
		shape._onChanged = Callback(&Collider::_OnShapeChanged, *this);
		return shape;
	}

	size_t GetShapeCount() const									{ return _shapes.GetSize(); }
	CollisionShape& GetShape(size_t index)							{ return *_shapes[index]; }
	const CollisionShape& GetShape(size_t index) const				{ return *_shapes[index]; }
	void RemoveShape(size_t index);
	void ClearShapes();

	void SetChannels(ECollisionChannels channels)					{ _channels = channels; }
	void AddChannels(ECollisionChannels channels)					{ _channels |= channels; }
	void RemoveChannels(ECollisionChannels channels)				{ _channels = (ECollisionChannels)(_channels & (~channels)); }
	bool CanCollideWithChannels(ECollisionChannels channels) const	{ return (_channels & channels) != ECollisionChannels::NONE; }
	bool CanCollideWith(const Collider& other) const				{ return CanCollideWithChannels(other._channels); }

	bool IntersectsRay(const Transform& transform, const Ray&, ECollisionChannels rayChannels, RaycastHitInformation&) const;

	bool BroadOverlapCheck(const Transform& transform, const Collider& other, const Transform& otherTransform) const;

	//The penetration vector will be our penetration into other if OVERLAPPING, and will be normalised if TOUCHING
	EOverlapResult NarrowOverlapCheck(
		const Transform& transform, 
		const Collider& other, const Transform& otherTransform, 
		const LineSegment* lineA, Vector3* out_PenetrationVector) const;
	
	float MinimumDistanceTo(
		const Transform& transform, 
		const Collider& other, const Transform& otherTransform, 
		Vector3& out_PointA, Vector3& out_PointB, 
		const LineSegment* lineA = nullptr) const;
};
