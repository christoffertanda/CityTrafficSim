#include "mouse.h"

static mouse_state Mouse = {};

void UpdateMouse(LPARAM LParam, WPARAM WParam)
{
	Mouse.X = (real32)GET_X_LPARAM(LParam);
	Mouse.Y = (real32)GET_Y_LPARAM(LParam);

	WORD KeyState = GET_KEYSTATE_WPARAM(WParam);

	// NOTE(Cristoffer): Left mouse button.
	if(KeyState == MK_LBUTTON)
	{
		Mouse.Button[MOUSE_BUTTON_LEFT] = 1;
	}
	else
	{
		Mouse.Button[MOUSE_BUTTON_LEFT] = 0;
	}

	// NOTE(Cristoffer): Right mouse button.
	if(KeyState == MK_RBUTTON)
	{
		Mouse.Button[MOUSE_BUTTON_RIGHT] = 1;
	}
	else
	{
		Mouse.Button[MOUSE_BUTTON_RIGHT] = 0;
	}

	// NOTE(Cristoffer): Middle mouse button.
	if(KeyState == MK_MBUTTON)
	{
		Mouse.Button[MOUSE_BUTTON_MIDDLE] = 1;
	}
	else
	{
		Mouse.Button[MOUSE_BUTTON_MIDDLE] = 0;
	}
}

void UpdateRawMouse(real32 X, real32 Y)
{
	Mouse.RawX = X;
	Mouse.RawY = Y;
}

real32 GetMouseX()
{
	return Mouse.X;
}

real32 GetMouseY()
{
	return Mouse.Y;
}

real32 GetMouseRawX()
{
	// TODO(Cristoffer): Clearing raw input to zero everytime its being fetched makes
	// it so that it only can get fetched once per frame. Think about this later down
	// the road!!
	// Problem solved by this: Removing "gliding" caused by old input.

	real32 Result = Mouse.RawX;

	Mouse.RawX = 0.0f;

	return Result;
}

real32 GetMouseRawY()
{
	real32 Result = Mouse.RawY;

	Mouse.RawY = 0.0f;

	return Result;
}

bool MousePressed(int32 ButtonCode)
{
	return Mouse.Button[ButtonCode];
}