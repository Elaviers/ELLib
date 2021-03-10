#pragma once
#include "EKeycode.hpp"
#include <ELCore/Function.hpp>
#include <ELCore/Hashmap.hpp>

enum class EInputAxis
{
	MOUSE_X, MOUSE_Y,
	LJOY_X, LJOY_Y,
	RJOY_X, RJOY_Y,
	LTRIGGER, RTRIGGER
};

class KeyBind
{
protected:
	KeyBind() {}

public:
	virtual ~KeyBind() {}

	virtual void KeyDown() {}
	virtual void KeyUp() {}
};

class KeyBind_Axis : public KeyBind
{
	float *_axis;
	float _displacement;

public:
	KeyBind_Axis(float *axis, float displacement) : _axis(axis), _displacement(displacement) {}
	virtual ~KeyBind_Axis() {}

	virtual void KeyDown()
	{
		*_axis += _displacement;
	}

	virtual void KeyUp()
	{
		*_axis -= _displacement;
	}
};

template <typename T>
requires Concepts::Callback<T>
class KeyBind_CallbackDown : public KeyBind
{
	T _callback;

public:
	KeyBind_CallbackDown(const T &callback) : _callback(callback) {}
	virtual ~KeyBind_CallbackDown() {}

	virtual void KeyDown()
	{
		_callback();
	}
};

template <typename T>
requires Concepts::Callback<T>
class KeyBind_CallbackUp : public KeyBind
{
	T _callback;

public:
	KeyBind_CallbackUp(const T& callback) : _callback(callback) {}
	virtual ~KeyBind_CallbackUp() {}

	virtual void KeyUp()
	{
		_callback();
	}
};

class InputManager
{
private:
	byte _keyStates[256];

	Hashmap<EKeycode, List<KeyBind*>> _keyBinds;
	Hashmap<EInputAxis, List<float*>> _axisBinds;

public:
	InputManager() : _keyStates() {}
	~InputManager();

	void Clear();

	void BindAxis(EInputAxis axis, float *axisPtr) { _axisBinds[axis].Add(axisPtr); }

	template <typename T>
	void BindKeyDown(EKeycode key, const T &callback) { _keyBinds[key].Add(new KeyBind_CallbackDown<T>(callback)); }
	
	template <typename T>
	void BindKeyUp(EKeycode key, const T &callback) { _keyBinds[key].Add(new KeyBind_CallbackUp<T>(callback)); }
	
	void BindKeyAxis(EKeycode key, float *axisPtr, float axisDisplacement) { _keyBinds[key].Add(new KeyBind_Axis(axisPtr, axisDisplacement)); }

	bool IsKeyDown(EKeycode key) const { return _keyStates[(byte)key] == 1; }

	void KeyDown(EKeycode);
	void KeyUp(EKeycode);

	void Reset();

	void ClearMouseInput();

	void AddMouseInput(float xDelta, float yDelta);
};
