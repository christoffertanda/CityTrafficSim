#include "window.h"

window::window(const char *Title, int32 Width, int32 Height) : 
	WindowInstance(GetModuleHandle(0))
{
	global_device_info::WindowWidth = Width;
	global_device_info::WindowHeight = Height;

	WNDCLASSEX WindowClass = { };
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_OWNDC;
	WindowClass.lpfnWndProc = WindowProcedure;
	WindowClass.cbClsExtra = 0;
	WindowClass.cbWndExtra = 0;
	WindowClass.hInstance = WindowInstance;
	WindowClass.hIcon = NULL;
	WindowClass.hCursor = NULL;
	WindowClass.hbrBackground = NULL;
	WindowClass.lpszMenuName = NULL;
	WindowClass.lpszClassName = "CitySimClass";
	WindowClass.hIconSm = NULL;

	RegisterClassEx(&WindowClass);

	int32 WindowStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

	RECT WindowRect = { 0, 0, Width, Height };
	AdjustWindowRect(&WindowRect, WindowStyle, FALSE);
	int32 DrawableRectWidth = WindowRect.right - WindowRect.left;
	int32 DrawableRectHeight = WindowRect.bottom - WindowRect.top;

	WindowHandle = CreateWindowEx(0, WindowClass.lpszClassName, Title, WindowStyle, CW_USEDEFAULT, CW_USEDEFAULT, DrawableRectWidth, DrawableRectHeight, 0, 0, GetModuleHandle(0), 0);

	if(!WindowHandle)
	{
		MessageBoxA(WindowHandle,
			ERROR_MESSAGE_0000,
			NULL,
			MB_OK | MB_ICONWARNING
			);

		return;
	}

	InitRawInputDevice();
	InitGraphicsDevice();

	ShowWindow(WindowHandle, SW_SHOW);
}

void window::InitRawInputDevice()
{
	#ifndef HID_USAGE_PAGE_GENERIC
	#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
	#endif
	#ifndef HID_USAGE_GENERIC_MOUSE
	#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
	#endif

	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = WindowHandle;
	RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
}

void window::InitGraphicsDevice()
{
	Direct3D.Init(WindowHandle);
}

HWND window::GetHandle() const
{
	return WindowHandle;
}

direct3d &window::GetGraphicsDevice()
{
	return Direct3D;
}

void window::SetTitle(const char *Title)
{
	SetWindowText(WindowHandle, Title);
}

void window::ClipMouseCursor(bool Clip)
{
	RECT ClientRect;
	GetClientRect(WindowHandle, &ClientRect);

	POINT LeftTop;
	LeftTop.x = ClientRect.left;
	LeftTop.y = ClientRect.top;

	POINT RightBottom;
	RightBottom.x = ClientRect.right;
	RightBottom.y = ClientRect.bottom;

	MapWindowPoints(WindowHandle, nullptr, &LeftTop, 1);
	MapWindowPoints(WindowHandle, nullptr, &RightBottom, 1);

	ClientRect.left = LeftTop.x;
	ClientRect.top = LeftTop.y;

	ClientRect.right = RightBottom.x;
	ClientRect.bottom = RightBottom.y;

	if(Clip == 1)
	{
		ClipCursor(&ClientRect);
	}
	else
	{
		ClipCursor(nullptr);
	}
}

void window::ShowMouseCursor(bool Show)
{
	ShowCursor(Show);
}