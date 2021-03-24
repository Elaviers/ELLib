#include "Collider.hpp"
#include "RaycastHitInformation.hpp"

Collider::Collider(ECollisionChannels channels, const CollisionShape& shape) : 
	_channels(channels), _shapes({ shape.Clone() })
{
	_shapes[0]->_onChanged = Callback(*this, &Collider::_OnShapeChanged);

	_OnShapeChanged();
}

Collider::Collider(ECollisionChannels channels, const Buffer<CollisionShape>& shapes) :
	_channels(channels)
{
	_shapes.SetSize(shapes.GetSize());
	for (size_t i = 0; i < _shapes.GetSize(); ++i)
	{
		_shapes[i] = shapes[i].Clone();
		_shapes[i]->_onChanged = Callback(*this, &Collider::_OnShapeChanged);
	}

	_OnShapeChanged();
}

Collider::Collider(const Collider& other) :
	_channels(other._channels)
{
	_shapes.SetSize(other._shapes.GetSize());
	for (size_t i = 0; i < _shapes.GetSize(); ++i)
	{
		_shapes[i] = other._shapes[i]->Clone();
		_shapes[i]->_onChanged = Callback(*this, &Collider::_OnShapeChanged);
	}

	_OnShapeChanged();
}

Collider::Collider(Collider&& other) noexcept :
	_channels(other._channels), _shapes(std::move(other._shapes)), _maxRadius(other._maxRadius)
{
	other._shapes.Clear();
}

Collider::~Collider()
{
	for (size_t i = 0; i < _shapes.GetSize(); ++i)
		delete _shapes[i];
}

Collider& Collider::operator=(const Collider& other)
{
	_channels = other._channels;

	for (CollisionShape* shape : _shapes)
		delete shape;

	_shapes.SetSize(other._shapes.GetSize());
	for (size_t i = 0; i < _shapes.GetSize(); ++i)
	{
		_shapes[i] = other._shapes[i]->Clone();
		_shapes[i]->_onChanged = Callback(*this, &Collider::_OnShapeChanged);
	}

	_OnShapeChanged();
	return *this;
}

Collider& Collider::operator=(Collider&& other) noexcept
{
	_channels = other._channels;
	_shapes = std::move(other._shapes);
	_maxRadius = other._maxRadius;

	return *this;
}

void Collider::_OnShapeChanged()
{
	_maxRadius = 0.f;
	for (size_t i = 0; i < GetShapeCount(); ++i)
	{
		const CollisionShape& shape = GetShape(i);
		_maxRadius = Maths::Max(_maxRadius, shape.GetTransform().GetPosition().Length() + shape.GetBoundingRadius());
	}
}

bool Collider::IntersectsRay(const Transform& transform, const Ray& ray, ECollisionChannels channels, RaycastHitInformation& result) const
{
	if (CanCollideWithChannels(channels))
		for (size_t i = 0; i < GetShapeCount(); ++i)
			if (GetShape(i).IntersectsRay(ray, result, transform))
				return true;

	return false;
}

inline Vector3 ScaleRelativeToPoint(const Vector3& x, const Vector3& point, float scale)
{
	return Vector3(
		((x.x - point.x) * scale) + point.x,
		((x.y - point.y) * scale) + point.y,
		((x.z - point.z) * scale) + point.z
	);
}

void Collider::RemoveShape(size_t index)
{
	delete _shapes[index];
	_shapes.RemoveIndex(index);
}

void Collider::ClearShapes()
{
	for (CollisionShape* shape : _shapes)
		delete shape;

	_shapes.Clear();
}

bool Collider::BroadOverlapCheck(const Transform& transform, const Collider& other, const Transform& otherTransform) const
{
	if (CanCollideWith(other))
	{
		float maxDist = GetBoundingRadius() * Maths::Max(transform.GetScale().Abs().GetData(), 3) + other.GetBoundingRadius() * Maths::Max(otherTransform.GetScale().Abs().GetData(), 3);
		return (transform.GetPosition() - otherTransform.GetPosition()).LengthSquared() < maxDist * maxDist;
	}

	return false;
}
