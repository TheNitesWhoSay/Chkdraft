#include "ClassWindow.h"
#include <sstream>
#include <iostream>

ClassWindow::ClassWindow() : hWndMDIClient(NULL), windowType(WindowTypes::None)
{
	WindowClassName().clear();
}

ClassWindow::~ClassWindow()
{
	if ( getHandle() != NULL )
		DestroyWindow(getHandle());
}

bool ClassWindow::RegisterWindowClass(UINT style, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground,
	LPCTSTR lpszMenuName, LPCTSTR lpszClassName, HICON hIconSm, bool isMDIChild)
{
	if ( isMDIChild )
	{
		return WindowsItem::RegisterWindowClass(style, hIcon, hCursor, hbrBackground,
			lpszMenuName, lpszClassName, hIconSm, SetupMDIChildProc);
	}
	else
	{
		return WindowsItem::RegisterWindowClass(style, hIcon, hCursor, hbrBackground,
			lpszMenuName, lpszClassName, hIconSm, SetupWndProc);
	}
}

bool ClassWindow::CreateClassWindow( DWORD dwExStyle, LPCSTR lpWindowName, DWORD dwStyle,
									 int x, int y, int nWidth, int nHeight,
									 HWND hWndParent, HMENU hMenu )
{
	if ( getHandle() == NULL && WindowClassName().length() > 0 )
	{
		windowType = WindowTypes::Regular;

		setHandle(CreateWindowEx(dwExStyle, WindowClassName().c_str(), lpWindowName,
			dwStyle, x, y, nWidth, nHeight,
			hWndParent, hMenu, GetModuleHandle(NULL), (LPVOID)this));

		if ( getHandle() != NULL )
		{
			WindowClassName().clear();
			return true;
		}
	}
	windowType = WindowTypes::None;
	return false;
}

bool ClassWindow::CreateMdiChild( LPCSTR lpWindowName, DWORD dwStyle,
								  int x, int y, int nWidth, int nHeight,
								  HWND hParent )
{
	if ( getHandle() == NULL && WindowClassName().length() > 0 )
	{
		windowType = WindowTypes::MDIChild;
		setHandle(CreateMDIWindow(WindowClassName().c_str(), lpWindowName, dwStyle,
			x, y, nWidth, nHeight,
			hParent, GetModuleHandle(NULL), (LPARAM)this));

		if ( getHandle() != NULL )
		{
			WindowClassName().clear();
			return true;
		}
	}
	windowType = WindowTypes::None;
	return false;
}

bool ClassWindow::BecomeMDIFrame( MdiClient &client, HANDLE hWindowMenu, UINT idFirstChild, DWORD dwStyle,
	int X, int Y, int nWidth, int nHeight, HWND hParent, HMENU hMenu )
{
	WindowTypes prev = windowType;
	windowType = WindowTypes::MDIFrame;
	if ( client.CreateMdiClient( hWindowMenu, idFirstChild, dwStyle,
								 X, Y, nWidth, nHeight, getHandle(), hMenu ) )
	{
		this->hWndMDIClient = client.getHandle();
		return true;
	}
	windowType = prev;
	return false;
}

LRESULT ClassWindow::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
	switch ( windowType )
	{
	case WindowTypes::MDIFrame: return DefFrameProc(hWnd, hWndMDIClient, WM_NOTIFY, idFrom, (LPARAM)nmhdr); break;
	case WindowTypes::MDIChild: return DefMDIChildProc(hWnd, WM_NOTIFY, idFrom, (LPARAM)nmhdr); break;
	}
	return DefWindowProc(hWnd, WM_NOTIFY, idFrom, (LPARAM)nmhdr);
}

LRESULT ClassWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch ( windowType )
	{
	case WindowTypes::MDIFrame: return DefFrameProc(hWnd, hWndMDIClient, WM_COMMAND, wParam, lParam); break;
	case WindowTypes::MDIChild: return DefMDIChildProc(hWnd, WM_COMMAND, wParam, lParam); break;
	}
	return DefWindowProc(hWnd, WM_COMMAND, wParam, lParam);
}

LRESULT ClassWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( windowType )
	{
	case WindowTypes::MDIFrame: return DefFrameProc(hWnd, hWndMDIClient, msg, wParam, lParam); break;
	case WindowTypes::MDIChild: return DefMDIChildProc(hWnd, msg, wParam, lParam); break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK ClassWindow::SetupWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( msg == WM_NCCREATE )
	{
		LONG classPtr = (LONG)((CREATESTRUCT*)lParam)->lpCreateParams;
		SetWindowLong(hWnd, GWL_USERDATA, classPtr);
		if ( GetWindowLong(hWnd, GWL_USERDATA) == classPtr && classPtr != 0 && SetWindowLong(hWnd, GWL_WNDPROC, (LONG)ForwardWndProc) != 0 )
			return ((ClassWindow*)classPtr)->WndProc(hWnd, msg, wParam, lParam);
		else
			return FALSE;
	}
	else
		return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK ClassWindow::SetupMDIChildProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( msg == WM_NCCREATE )
	{
		LONG classPtr = (LONG)((MDICREATESTRUCT*)((CREATESTRUCT*)lParam)->lpCreateParams)->lParam;
		SetWindowLong(hWnd, GWL_USERDATA, classPtr);
		if ( GetWindowLong(hWnd, GWL_USERDATA) == classPtr  && classPtr != 0 && SetWindowLong(hWnd, GWL_WNDPROC, (LONG)ForwardWndProc) != 0 )
			return ((ClassWindow*)classPtr)->WndProc(hWnd, msg, wParam, lParam);
		else
			return FALSE;
	}
	else
		return DefMDIChildProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK ClassWindow::ForwardWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_NOTIFY: return ((ClassWindow*)GetWindowLong(hWnd, GWL_USERDATA))->Notify(hWnd, wParam, (NMHDR*)lParam); break;
		case WM_COMMAND: return ((ClassWindow*)GetWindowLong(hWnd, GWL_USERDATA))->Command(hWnd, wParam, lParam); break;
	}
	return ((ClassWindow*)GetWindowLong(hWnd, GWL_USERDATA))->WndProc(hWnd, msg, wParam, lParam);
}
