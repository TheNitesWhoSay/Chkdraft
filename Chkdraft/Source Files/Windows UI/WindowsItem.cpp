#include "WindowsItem.h"

#include <list>
#include <string>

std::list<std::string> WindowsItem::registeredClasses; // Obligatory definition of static variable

WindowsItem::WindowsItem() : hWnd(NULL)
{

}

HWND WindowsItem::getHandle()
{
	//if ( this != nullptr )
		return hWnd;
	//else
	//	return NULL;
}

bool WindowsItem::operator==(HWND hWnd)
{
	return getHandle() == hWnd;
}

void WindowsItem::setHandle(HWND hWnd)
{
	this->hWnd = hWnd;
}

bool WindowsItem::RegisterWindowClass(UINT style, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground,
	LPCTSTR lpszMenuName, LPCTSTR lpszClassName, HICON hIconSm, WNDPROC wndProc)
{
	if ( getHandle() == NULL && windowClassName.length() <= 0 )
	{
		if ( WindowClassIsRegistered(lpszClassName) )
		{
			try { windowClassName = lpszClassName; }
			catch ( std::exception ) { return false; }
			return true;
		}
		else
		{
			WNDCLASSEX wc = {};
			wc.cbSize = sizeof(WNDCLASSEX);
			wc.style = style;
			wc.lpfnWndProc = wndProc;
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

bool WindowsItem::WindowClassIsRegistered(LPCTSTR lpszClassName)
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

int WindowsItem::CompareLvItems(LPARAM index1, LPARAM index2)
{
	return 0; // No sort unless this method is overridden
}

int CALLBACK WindowsItem::ForwardCompareLvItems(LPARAM index1, LPARAM index2, LPARAM lParam)
{
	return ((WindowsItem*)lParam)->CompareLvItems(index1, index2);
}

std::string &WindowsItem::WindowClassName()
{
	return windowClassName;
}

bool WindowsItem::getWindowRect(RECT &rect)
{
	return ::GetWindowRect(getHandle(), &rect) != 0;
}

bool WindowsItem::getClientRect(RECT &rect)
{
	return ::GetClientRect(getHandle(), &rect) != 0;
}

int WindowsItem::Width()
{
	RECT rect = {};
	::GetWindowRect(getHandle(), &rect);
	return rect.right - rect.left;
}

int WindowsItem::Height()
{
	RECT rect = {};
	::GetWindowRect(getHandle(), &rect);
	return rect.bottom - rect.top;
}

int WindowsItem::Left()
{
	RECT rect = {};
	::GetWindowRect(getHandle(), &rect);
	::MapWindowPoints(HWND_DESKTOP, ::GetParent(getHandle()), (LPPOINT)&rect, 2);
	return rect.left;
}

int WindowsItem::Top()
{
	RECT rect = {};
	::GetWindowRect(getHandle(), &rect);
	::MapWindowPoints(HWND_DESKTOP, ::GetParent(getHandle()), (LPPOINT)&rect, 2);
	return rect.top;
}

int WindowsItem::Right()
{
	RECT rect = {};
	::GetWindowRect(getHandle(), &rect);
	::MapWindowPoints(HWND_DESKTOP, ::GetParent(getHandle()), (LPPOINT)&rect, 2);
	return rect.right;
}

int WindowsItem::Bottom()
{
	RECT rect = {};
	::GetWindowRect(getHandle(), &rect);
	::MapWindowPoints(HWND_DESKTOP, ::GetParent(getHandle()), (LPPOINT)&rect, 2);
	return rect.bottom;
}

LONG WindowsItem::cliWidth()
{
	RECT rect;
	if ( ::GetClientRect(getHandle(), &rect) != 0 )
		return rect.right - rect.left;
	else
		return 0;
}

LONG WindowsItem::cliHeight()
{
	RECT rect;
	if ( ::GetClientRect(getHandle(), &rect) != 0 )
		return rect.bottom - rect.top;
	else
		return 0;
}

HDC WindowsItem::getDC()
{
	return ::GetDC(getHandle());
}

bool WindowsItem::isEnabled()
{
	return ::IsWindowEnabled(getHandle()) != 0;
}

BOOL CALLBACK SetFont(HWND hWnd, LPARAM hFont) // Callback function for ReplaceChildFonts
{
	::SendMessage(hWnd, WM_SETFONT, hFont, TRUE);
	return TRUE;
}

void WindowsItem::SetFont(HFONT hFont, bool redrawImmediately)
{
	if ( redrawImmediately )
		::SendMessage(getHandle(), WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
	else
		::SendMessage(getHandle(), WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
}

void WindowsItem::ReplaceChildFonts(HFONT hFont)
{
	::EnumChildWindows(getHandle(), (WNDENUMPROC)::SetFont, (LPARAM)hFont);
}

void WindowsItem::LockCursor()
{
	if ( this != nullptr )
	{
		if ( getHandle() == NULL )
			::ClipCursor(NULL);
		else
		{
			RECT clientRect, clipRect;
			::GetClientRect(getHandle(), &clientRect);

			POINT upperLeft, lowerRight;
			upperLeft.x = 0;
			upperLeft.y = 0;
			lowerRight.x = clientRect.right - 1;
			lowerRight.y = clientRect.bottom - 1;

			::ClientToScreen(getHandle(), &upperLeft);
			::ClientToScreen(getHandle(), &lowerRight);

			clipRect.left = upperLeft.x;
			clipRect.top = upperLeft.y;
			clipRect.bottom = lowerRight.y;
			clipRect.right = lowerRight.x;

			::ClipCursor(&clipRect);
		}
	}
}

void WindowsItem::UnlockCursor()
{
	::ClipCursor(NULL);
}

void WindowsItem::TrackMouse(DWORD hoverTime)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_HOVER;
	tme.hwndTrack = getHandle();
	tme.dwHoverTime = hoverTime;
	::TrackMouseEvent(&tme);
}

bool WindowsItem::SetParent(HWND hParent)
{
	return ::SetParent(getHandle(), hParent) != NULL;
}

void WindowsItem::SetRedraw(bool autoRedraw)
{
	if ( autoRedraw )
		::SendMessage(getHandle(), WM_SETREDRAW, TRUE, NULL);
	else
		::SendMessage(getHandle(), WM_SETREDRAW, FALSE, NULL);
}

void WindowsItem::RedrawThis()
{
	::RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);
}

void WindowsItem::MoveTo(int x, int y)
{
	::SetWindowPos(getHandle(), NULL, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
}

void WindowsItem::SetPos(int x, int y, int width, int height)
{
	::SetWindowPos(getHandle(), NULL, x, y, width, height, SWP_NOACTIVATE | SWP_NOZORDER);
}

void WindowsItem::ShowNormal()
{
	::ShowWindow(getHandle(), SW_SHOWNORMAL);
}

void WindowsItem::Show()
{
	::ShowWindow(getHandle(), SW_SHOW);
}

void WindowsItem::Hide()
{
	::ShowWindow(getHandle(), SW_HIDE);
}

void WindowsItem::SetSmallIcon(HANDLE hIcon)
{
	::SendMessage(getHandle(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
}

bool WindowsItem::SetTitle(const std::string& newTitle)
{
	return ::SetWindowText(getHandle(), newTitle.c_str()) != 0;
}
bool WindowsItem::SetTitle(const char* newTitle)
{
	return ::SetWindowText(getHandle(), newTitle) != 0;
}

bool WindowsItem::ReleaseDC(HDC hDC)
{
	return ::ReleaseDC(getHandle(), hDC) != 0;
}

void WindowsItem::FocusThis()
{
	::SetFocus(getHandle());
}

void WindowsItem::UpdateWindow()
{
	::UpdateWindow(getHandle());
}

void WindowsItem::DisableThis()
{
	::EnableWindow(getHandle(), FALSE);
}

void WindowsItem::EnableThis()
{
	::EnableWindow(getHandle(), TRUE);
}

void WindowsItem::SetWidth(int newWidth)
{
	::SetWindowPos(getHandle(), NULL, 0, 0, newWidth, Height(), SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}
