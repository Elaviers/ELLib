#include "InputManager.hpp"
#include <ELCore/String.hpp>
#include <Windows.h>

#define KEYNAME(KEY, NAME) {EKeycode::KEY, #NAME}

const struct KeyName { EKeycode key; String name; } keyNames[] =
{
	KEYNAME(A, a), KEYNAME(B, b), KEYNAME(C, c), KEYNAME(D, d), KEYNAME(E, e), KEYNAME(F, f), KEYNAME(G, g), KEYNAME(H, h), KEYNAME(I, i), KEYNAME(J, j), KEYNAME(K, k), KEYNAME(L, l), KEYNAME(M, m),
	KEYNAME(N, n), KEYNAME(O, o), KEYNAME(P, p), KEYNAME(Q, q), KEYNAME(R, r), KEYNAME(S, s), KEYNAME(T, t), KEYNAME(U, u), KEYNAME(V, v), KEYNAME(W, w), KEYNAME(X, x), KEYNAME(Y, y), KEYNAME(Z, z),
	KEYNAME(NUM0, 0), KEYNAME(NUM1, 1),KEYNAME(NUM2, 2),KEYNAME(NUM3, 3),KEYNAME(NUM4, 4),KEYNAME(NUM5, 5),KEYNAME(NUM6, 6),KEYNAME(NUM7, 7),KEYNAME(NUM8, 8), KEYNAME(NUM9, 9),
	KEYNAME(F1, f1), KEYNAME(F2, f2), KEYNAME(F3, f3), KEYNAME(F4, f4), KEYNAME(F5, f5), KEYNAME(F6, f6), KEYNAME(F7, f7), KEYNAME(F8, f8), KEYNAME(F9, f9), KEYNAME(F10, f10), KEYNAME(F11, f11), KEYNAME(F12, f12),

	KEYNAME(SPACE, space),
	KEYNAME(LCTRL, lctrl), KEYNAME(LSHIFT, lshift), KEYNAME(LALT, lalt),
	KEYNAME(RCTRL, rctrl), KEYNAME(RSHIFT, rshift), KEYNAME(RALT, ralt),
	KEYNAME(ENTER, enter),
	KEYNAME(ESCAPE, escape),
	KEYNAME(CAPSLOCK, caps),
	KEYNAME(TAB, tab),
	KEYNAME(BACKSPACE, backspace),

	KEYNAME(SEMICOLON, semicolon),
	KEYNAME(SLASH, / ),
	KEYNAME(TILDE, ~),
	KEYNAME(SQBRACKETLEFT,[),
	KEYNAME(BACKSLASH, \\),
	KEYNAME(SQBRACKETRIGHT,]),
	{ EKeycode::APOSTROPHE, "\'" },
	{ EKeycode::COMMA, ","},
	{ EKeycode::PERIOD, '.'},

	KEYNAME(INSERT, insert), KEYNAME(HOME, home), KEYNAME(PAGEUP, pageup),
	KEYNAME(DEL, delete), KEYNAME(END, end), KEYNAME(PAGEDOWN, pagedown),

	KEYNAME(UP, up), KEYNAME(RIGHT, right), KEYNAME(LEFT, left), KEYNAME(DOWN, down),

	KEYNAME(MOUSE_LEFT, mouseleft),
	KEYNAME(MOUSE_MIDDLE, mousemiddle),
	KEYNAME(MOUSE_RIGHT, mouseright),
	KEYNAME(MOUSE_4, mouse4),
	KEYNAME(MOUSE_5, mouse5),
	KEYNAME(MOUSE_SCROLLUP, scrollup),
	KEYNAME(MOUSE_SCROLLDOWN, scrolldown)
};

const uint16 keyCount = sizeof(keyNames) / sizeof(KeyName);

String KeyToString(EKeycode key)
{
	for (uint16 i = 0; i < keyCount; ++i)
		if (keyNames[i].key == key)
			return keyNames[i].name;

	return "";
}

EKeycode StringToKey(const char* string)
{
	size_t length = StringLength(string);

	for (uint16 i = 0; i < keyCount; ++i)
		if (StringsEqual(string, keyNames[i].name.GetData()))
			return keyNames[i].key;

	return EKeycode::NONE;
}

InputManager::~InputManager()
{
	_keyBinds.ForEach([](const EKeycode&, List<KeyBind*>& binds)
		{
			for (KeyBind* bind : binds)
				delete bind;
		});
}

void InputManager::Clear()
{
	_keyBinds.ForEach([](const EKeycode&, List<KeyBind*>& binds)
		{
			for (KeyBind* bind : binds)
				delete bind;
		});

	_keyBinds.Clear();
	_axisBinds.Clear();
}

void InputManager::KeyDown(EKeycode key)
{
	if (_keyStates[(byte)key] == 0)
	{
		_keyStates[(byte)key] = 1;

		List<KeyBind*>* binds = _keyBinds.Get(key);
		if (binds)
			for (KeyBind* bind : *binds)
				bind->KeyDown();
	}
}

void InputManager::KeyUp(EKeycode key)
{
	if (_keyStates[(byte)key])
	{
		_keyStates[(byte)key] = 0;

		List<KeyBind*>* binds = _keyBinds.Get(key);
		if (binds)
			for (KeyBind* bind : *binds)
				bind->KeyUp();
	}
}

void InputManager::ClearMouseInput()
{
	List<float*>* xAxes = _axisBinds.Get(EInputAxis::MOUSE_X);
	List<float*>* yAxes = _axisBinds.Get(EInputAxis::MOUSE_Y);

	if (xAxes)
		for (float* axis : *xAxes)
			*axis = 0.f;

	if (yAxes)
		for (float* axis : *yAxes)
			*axis = 0.f;
}

void InputManager::AddMouseInput(float x, float y)
{
	List<float*>* xAxes = _axisBinds.Get(EInputAxis::MOUSE_X);
	List<float*>* yAxes = _axisBinds.Get(EInputAxis::MOUSE_Y);

	if (xAxes)
		for (float* axis : *xAxes)
			*axis += x;

	if (yAxes)
		for (float* axis : *yAxes)
			*axis += y;
}

void InputManager::Reset()
{
	byte* keystates = _keyStates;
	_keyBinds.ForEach([&keystates](const EKeycode& kc, List<KeyBind*>& binds) { if (keystates[(size_t)kc] != 0) for (KeyBind* bind : binds) bind->KeyUp(); });
	_axisBinds.ForEach([](const EInputAxis&, List<float*>& axes) { for (float* axis : axes) *axis = 0.f; });

	for (int i = 0; i < 256; ++i)
		_keyStates[i] = 0;
}
