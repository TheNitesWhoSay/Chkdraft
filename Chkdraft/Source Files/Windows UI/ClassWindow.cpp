#include "ClassWindow.h"
#include <sstream>
#include <iostream>

std::list<std::string> ClassWindow::registeredClasses; // Obligatory definition of static variable

ClassWindow::ClassWindow() : windowHandle(NULL), windowType(WindowTypes::None)
{
	windowClassName.clear();
}

ClassWindow::~ClassWindow()
{
	if ( windowHandle != NULL )
		DestroyWindow(windowHandle);
}

bool ClassWindow::CreateThis()
{
	return false;
}

bool ClassWindow::DestroyThis()
{
	return false;
}

LRESULT ClassWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( windowType == WindowTypes::MDIChild )
		return DefMDIChildProc(hWnd, msg, wParam, lParam);
	else
		return DefWindowProc(hWnd, msg, wParam, lParam);
}

BOOL ClassWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefDlgProc(hWnd, msg, wParam, lParam);
}

int ClassWindow::CompareLvItems(LPARAM index1, LPARAM index2)
{
	return 0; // No sort unless this method is overridden
}

bool ClassWindow::RegisterWindowClass( UINT style, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground,
									   LPCTSTR lpszMenuName, LPCTSTR lpszClassName, HICON hIconSm, bool isMDIChild )
{
	if ( windowHandle == NULL && windowClassName.length() <= 0 )
	{
		if ( WindowClassIsRegistered(lpszClassName) )
		{
			try { windowClassName = lpszClassName; } catch ( std::exception ) { return false; }
			return true;
		}
		else
		{
			WNDCLASSEX wc = { };
			wc.cbSize = sizeof(WNDCLASSEX);
			wc.style = style;
			if ( isMDIChild )
				wc.lpfnWndProc = SetupMDIChildProc;
			else
				wc.lpfnWndProc = SetupWndProc;
			wc.hInstance = GetModuleHandle(NULL);
			wc.hIcon = hIcon;
			wc.hCursor = hCursor;
			wc.hbrBackground = hbrBackground;
			wc.lpszMenuName = lpszMenuName;
			wc.lpszClassName = lpszClassName;
			wc.hIconSm = hIconSm;
			if ( RegisterClassEx(&wc) != 0 )
			{
				try { windowClassName = lpszClassName; registeredClasses.push_back(windowClassName); }
				catch ( std::exception ) { return false; }
				return true;
			}
		}
	}
	return false;
}

bool ClassWindow::WindowClassIsRegistered(LPCTSTR lpszClassName)
{
	try { windowClassName = lpszClassName; }
	catch ( std::exception ) { return false; }
	if ( std::find(registeredClasses.begin(), registeredClasses.end(), windowClassName) == registeredClasses.end() )
	{
		windowClassName.clear();
		return false;
	}
	else
		return true;
}

bool ClassWindow::CreateClassWindow( DWORD dwExStyle, LPCSTR lpWindowName, DWORD dwStyle,
									 int x, int y, int nWidth, int nHeight,
									 HWND hWndParent, HMENU hMenu )
{
	if ( windowHandle == NULL && windowClassName.length() > 0  )
	{
		windowHandle = CreateWindowEx( dwExStyle, windowClassName.c_str(), lpWindowName,
									   dwStyle, x, y, nWidth, nHeight,
									   hWndParent, hMenu, GetModuleHandle(NULL), (LPVOID)this );

		if ( windowHandle != NULL )
		{
			windowClassName.clear();
			return true;
		}
	}
	return false;
}

bool ClassWindow::CreateMdiChild( LPCSTR lpWindowName, DWORD dwStyle,
								  int x, int y, int nWidth, int nHeight,
								  HWND hParent )
{
	if ( windowHandle == NULL && windowClassName.length() > 0 )
	{
		windowType = WindowTypes::MDIChild;
		windowHandle = CreateMDIWindow( windowClassName.c_str(), lpWindowName, dwStyle,
										x, y, nWidth, nHeight,
										hParent, GetModuleHandle(NULL), (LPARAM)this );

		if ( windowHandle != NULL )
		{
			windowClassName.clear();
			return true;
		}
	}
	windowType = WindowTypes::None;
	return false;
}

bool ClassWindow::CreateModelessDialog(LPCSTR lpTemplateName, HWND hWndParent)
{
	if ( windowHandle == NULL )
		windowHandle = CreateDialogParam(GetModuleHandle(NULL), lpTemplateName, hWndParent, SetupDialogProc, (LPARAM)this);

	return windowHandle != NULL;
}

INT_PTR ClassWindow::CreateDialogBox(LPCSTR lpTemplateName, HWND hWndParent)
{
	return DialogBoxParam(GetModuleHandle(NULL), lpTemplateName, hWndParent, SetupDialogProc, (LPARAM)this);
}

HWND ClassWindow::getHandle()
{
	return windowHandle;
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

BOOL CALLBACK ClassWindow::SetupDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( msg == WM_INITDIALOG )
	{
		LONG classPtr = lParam;
		SetWindowLong(hWnd, DWL_USER, classPtr);
		if ( GetWindowLong(hWnd, DWL_USER) == classPtr  && classPtr != 0 && SetWindowLong(hWnd, DWL_DLGPROC, (LONG)ForwardDlgProc) != 0 )
			return ((ClassWindow*)classPtr)->DlgProc(hWnd, msg, wParam, lParam);
		else
			return FALSE;
	}
	else
		return FALSE;
}

LRESULT CALLBACK ClassWindow::ForwardWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ((ClassWindow*)GetWindowLong(hWnd, GWL_USERDATA))->WndProc(hWnd, msg, wParam, lParam);
}

BOOL CALLBACK ClassWindow::ForwardDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ((ClassWindow*)GetWindowLong(hWnd, DWL_USER))->DlgProc(hWnd, msg, wParam, lParam);
}

int CALLBACK ClassWindow::ForwardCompareLvItems(LPARAM index1, LPARAM index2, LPARAM lParam)
{
	return ((ClassWindow*)lParam)->CompareLvItems(index1, index2);
}

bool ClassWindow::DestroyDialog()
{
	if ( windowHandle != NULL && EndDialog(windowHandle, IDCLOSE) != 0 )
	{
		windowHandle = NULL;
		return true;
	}
	else
		return false;
}
