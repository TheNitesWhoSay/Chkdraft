#include "WindowControl.h"

WindowControl::WindowControl() : controlHandle(NULL), defaultProc(NULL)
{

}

WindowControl::~WindowControl()
{
	
}

HWND WindowControl::getHandle()
{
	return controlHandle;
}

bool WindowControl::getClientRect(RECT &rect)
{
	return ::GetClientRect(controlHandle, &rect) != 0;
}

HDC WindowControl::getDC()
{
	return ::GetDC(controlHandle);
}

bool WindowControl::operator==(HWND hWnd)
{
	return controlHandle == hWnd;
}

bool WindowControl::FindThis(HWND hParent, u32 controlId)
{
	controlHandle = GetDlgItem(hParent, controlId);
	return controlHandle != NULL;
}

bool WindowControl::DestroyThis()
{
	if ( controlHandle != NULL && DestroyWindow(controlHandle) == TRUE )
	{
		controlHandle = NULL;
		defaultProc = NULL;
		return true;
	}
	else
		return false;
}

bool WindowControl::ReleaseDC(HDC hDC)
{
	return ::ReleaseDC(controlHandle, hDC) != 0;
}

void WindowControl::FocusThis()
{
	SetFocus(controlHandle);
}

void WindowControl::DisableThis()
{
	EnableWindow(controlHandle, FALSE);
}

void WindowControl::EnableThis()
{
	EnableWindow(controlHandle, TRUE);
}

bool WindowControl::isEnabled()
{
	return IsWindowEnabled(controlHandle) != 0;
}

void WindowControl::SetRedraw(bool autoRedraw)
{
	if ( autoRedraw )
		SendMessage(controlHandle, WM_SETREDRAW, TRUE, NULL);
	else
		SendMessage(controlHandle, WM_SETREDRAW, FALSE, NULL);
}

void WindowControl::RedrawThis()
{
	RedrawWindow(controlHandle, NULL, NULL, RDW_INVALIDATE);
}

void WindowControl::MoveTo(int x, int y)
{
	SetWindowPos(controlHandle, NULL, x, y, 0, 0, SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOSIZE);
}

void WindowControl::SetPos(int x, int y, int width, int height)
{
	SetWindowPos(controlHandle, NULL, x, y, width, height, SWP_NOACTIVATE|SWP_NOZORDER);
}

int WindowControl::Width()
{
	RECT rcControl = { };
	GetWindowRect(controlHandle, &rcControl);
	return rcControl.right - rcControl.left;
}

int WindowControl::Height()
{
	RECT rcControl = { };
	GetWindowRect(controlHandle, &rcControl);
	return rcControl.bottom - rcControl.top;
}

int WindowControl::Left()
{
	RECT rcControl = { };
	GetWindowRect(controlHandle, &rcControl);
	MapWindowPoints(HWND_DESKTOP, GetParent(controlHandle), (LPPOINT) &rcControl, 2);
	return rcControl.left;
}

int WindowControl::Top()
{
	RECT rcControl = { };
	GetWindowRect(controlHandle, &rcControl);
	MapWindowPoints(HWND_DESKTOP, GetParent(controlHandle), (LPPOINT) &rcControl, 2);
	return rcControl.top;
}

int WindowControl::Right()
{
	RECT rcControl = { };
	GetWindowRect(controlHandle, &rcControl);
	MapWindowPoints(HWND_DESKTOP, GetParent(controlHandle), (LPPOINT) &rcControl, 2);
	return rcControl.right;
}

int WindowControl::Bottom()
{
	RECT rcControl = { };
	GetWindowRect(controlHandle, &rcControl);
	MapWindowPoints(HWND_DESKTOP, GetParent(controlHandle), (LPPOINT) &rcControl, 2);
	return rcControl.bottom;
}

bool WindowControl::CreateControl( DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
								   int x, int y, int nWidth, int nHeight,
								   HWND hWndParent, HMENU hMenu, bool redirectProc )
{
	controlHandle = CreateWindowExA( dwExStyle, lpClassName, lpWindowName,
									 dwStyle, x, y, nWidth, nHeight,
									 hWndParent, hMenu, GetModuleHandle(NULL), (LPVOID)this );

	if ( controlHandle != NULL && redirectProc && !RedirectProc() )
		DestroyThis();

	return controlHandle != NULL;
}

bool WindowControl::RedirectProc()
{
	if ( controlHandle != NULL )
	{
		SetWindowLong(controlHandle, GWL_USERDATA, (LONG)this);
		defaultProc = (WNDPROC)GetWindowLong(controlHandle, GWL_WNDPROC);
		return defaultProc != 0 &&
			   GetWindowLong(controlHandle, GWL_USERDATA) == (LONG)this &&
			   SetWindowLong(controlHandle, GWL_WNDPROC, (LONG)&ForwardControlProc) != 0;
	}
	else
		return false;
}

LRESULT WindowControl::CallDefaultProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return CallWindowProc(defaultProc, hWnd, msg, wParam, lParam);
}

bool WindowControl::CreateThis(HWND hParent, int x, int y, int width, int height)
{
	return false;
}

LRESULT WindowControl::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return CallWindowProc(defaultProc, hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK WindowControl::ForwardControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ((WindowControl*)GetWindowLong(hWnd, GWL_USERDATA))->ControlProc(hWnd, msg, wParam, lParam);
}
