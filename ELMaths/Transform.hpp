#pragma once
#include "Vector3.hpp"
#include "Matrix4.hpp"
#include "Rotation.hpp"
#include <ELCore/Function.hpp>

class ByteReader;
class ByteWriter;

class Transform
{
	Vector3 _position;
	Rotation _rotation;
	Vector3 _scale;

	void _Update()
	{
		_matrixStatus = EMatrixStatus::EMPTY;
	}

	Matrix4 _matrix;
	enum class EMatrixStatus
	{
		EMPTY,
		TRANSFORM,
		INVERSETRANSFORM
	} _matrixStatus;

	void _MakeMatrix(Matrix4&) const;
	void _MakeInverseMatrix(Matrix4&) const;

public:
	Transform(const Vector3& position, const Rotation& rotation = Rotation(), const Vector3& scale = Vector3(1, 1, 1), const Callback& callback = Callback()) :
		_position(position), 
		_rotation(rotation), 
		_scale(scale), 
		_matrixStatus(EMatrixStatus::EMPTY) 
	{
	}

	Transform(const Callback& callback = Callback()) : Transform(Vector3(), Rotation(), Vector3(1, 1, 1), callback) {}
	
	Transform(const Transform& other) : 
		_position(other._position), 
		_rotation(other._rotation), 
		_scale(other._scale), 
		_matrix(other._matrix),
		_matrixStatus(other._matrixStatus)
	{}

	Transform(const Transform&& other) noexcept :
		_position(other._position),
		_rotation(other._rotation),
		_scale(other._scale),
		_matrix(other._matrix),
		_matrixStatus(other._matrixStatus)
	{}

	~Transform() {}

	Transform& operator=(const Transform& other)
	{
		_position = other._position;
		_rotation = other._rotation;
		_scale = other._scale;
		_matrix = other._matrix;
		_matrixStatus = other._matrixStatus;
		return *this;
	}

	static const Transform& Identity() { static Transform t; return t; }

	const Vector3& GetPosition() const			{ return _position; }
	const Rotation& GetRotation() const			{ return _rotation; }
	const Vector3& GetScale() const				{ return _scale; }

	void SetPosition(const Vector3 &position)	{ _position = position; _Update(); }
	void SetRotation(const Rotation &rotation)	{ _rotation = rotation; _Update(); }
	void SetScale(const Vector3 &scale)			{ _scale = scale;		_Update(); }

	void Move(const Vector3 &delta)				{ _position += delta;				_Update(); }
	void Rotate(const Rotation &delta)			{ _rotation = delta * _rotation;	_Update(); }

	void AddRotation(const Vector3& euler)		{ _rotation += euler; _Update(); }

	Matrix4 GetMatrix() const;
	Matrix4 GetInverseMatrix() const;
	Matrix4 MakeMatrix() const;
	Matrix4 MakeInverseMatrix() const;

	Vector3 GetForwardVector() const			{ return _rotation.GetQuat().GetForwardVector(); }
	Vector3 GetRightVector() const				{ return _rotation.GetQuat().GetRightVector(); }
	Vector3 GetUpVector() const					{ return _rotation.GetQuat().GetUpVector(); }

	Transform& operator*=(const Transform &other);
	Transform operator*(const Transform &other) const
	{
		Transform t = *this;
		t *= other;
		return t;
	}

	Transform Inverse() const { return Transform(_position * -1.f, _rotation.Inverse(), 1.f / _scale); }

	void Write(ByteWriter &buffer) const;
	void Read(ByteReader &buffer);
};
