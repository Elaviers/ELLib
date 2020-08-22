#pragma once
#include "EKeycode.hpp"
#include <ELCore/FunctionPointer.hpp>
#include <ELCore/Hashmap.hpp>

enum class EAxis
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

class KeyBind_CallbackDown : public KeyBind
{
	Callback _callback;

public:
	KeyBind_CallbackDown(const Callback &callback) : _callback(callback) {}
	virtual ~KeyBind_CallbackDown() {}

	virtual void KeyDown()
	{
		_callback.TryCall();
	}
};

class KeyBind_CallbackUp : public KeyBind
{
	Callback _callback;

public:
	KeyBind_CallbackUp(const Callback& callback) : _callback(callback) {}
	virtual ~KeyBind_CallbackUp() {}

	virtual void KeyUp()
	{
		_callback.TryCall();
	}
};

class InputManager
{
private:
	byte _keyStates[256];

	Hashmap<EKeycode, List<KeyBind*>> _keyBinds;
	Hashmap<EAxis, List<float*>> _axisBinds;

public:
	InputManager() : _keyStates() {}
	~InputManager();

	void Clear();

	void BindAxis(EAxis axis, float *axisPtr) { _axisBinds[axis].Add(axisPtr); }

	void BindKeyDown(EKeycode key, const Callback &callback) { _keyBinds[key].Add(new KeyBind_CallbackDown(callback)); }
	void BindKeyUp(EKeycode key, const Callback &callback) { _keyBinds[key].Add(new KeyBind_CallbackUp(callback)); }
	void BindKeyAxis(EKeycode key, float *axisPtr, float axisDisplacement) { _keyBinds[key].Add(new KeyBind_Axis(axisPtr, axisDisplacement)); }

	bool IsKeyDown(EKeycode key) const { return _keyStates[(byte)key] == 1; }

	void KeyDown(EKeycode);
	void KeyUp(EKeycode);

	void Reset();

	void ClearMouseInput();

	//xDelta and yDelta are in pixels
	void AddMouseInput(short xDelta, short yDelta);
};
