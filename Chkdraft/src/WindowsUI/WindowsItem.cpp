#include "WindowsItem.h"
#include <CommCtrl.h>
#include <memory>
#include <iostream>

std::list<std::string> WindowsItem::registeredClasses; // Obligatory definition of static variable

WindowsItem::WindowsItem() : windowsItemHandle(NULL), tooltipHandle(NULL), paintWidth(0), paintHeight(0),
    paintDc(NULL), paintFinalDc(NULL), paintMemBitmap(NULL), paintStatus(PaintStatus::NotPainting)
{
    paintRect.left = 0;
    paintRect.top = 0;
    paintRect.right = 0;
    paintRect.bottom = 0;
}

void WindowsItem::DestroyThis()
{
    if ( getHandle() != NULL )
        ::DestroyWindow(getHandle());
    windowsItemHandle = NULL;

    if ( tooltipHandle != NULL )
        ::DestroyWindow(tooltipHandle);
    tooltipHandle = NULL;

    paintWidth = 0;
    paintHeight = 0;
    paintDc = NULL;
    paintFinalDc = NULL;
    paintMemBitmap = NULL;
    paintStatus = PaintStatus::NotPainting;
    paintRect.left = 0;
    paintRect.top = 0;
    paintRect.right = 0;
    paintRect.bottom = 0;
}

HWND WindowsItem::getHandle()
{
    if ( this != nullptr )
        return windowsItemHandle;
    else
        return NULL;
}

HWND WindowsItem::getParent()
{
    return ::GetParent(getHandle());
}

bool WindowsItem::operator==(HWND hWnd)
{
    return getHandle() == hWnd;
}

void WindowsItem::setHandle(HWND hWnd)
{
    windowsItemHandle = hWnd;
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

int WindowsItem::CompareLvItems(LPARAM, LPARAM)
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

HDC WindowsItem::StartSimplePaint()
{
    if ( paintStatus == PaintStatus::NotPainting )
    {
        getClientRect(paintRect);
        paintWidth = paintRect.right - paintRect.left;
        paintHeight = paintRect.bottom - paintRect.top;
        paintDc = getDC();
        paintStatus = PaintStatus::SimplePaint;
        return paintDc;
    }
    return NULL;
}

HDC WindowsItem::StartBufferedPaint()
{
    if ( paintStatus == PaintStatus::NotPainting )
    {
        getClientRect(paintRect);
        paintWidth = paintRect.right - paintRect.left;
        paintHeight = paintRect.bottom - paintRect.top;
        paintFinalDc = getDC();
        paintDc = ::CreateCompatibleDC(paintFinalDc);
        paintMemBitmap = ::CreateCompatibleBitmap(paintFinalDc, paintWidth, paintHeight);
        ::SelectObject(paintDc, paintMemBitmap);
        paintStatus = PaintStatus::BufferedPaint;
        return paintDc;
    }
    return NULL;
}

HDC WindowsItem::GetPaintDc()
{
    return paintDc;
}

void WindowsItem::EndPaint()
{
    if ( paintStatus == PaintStatus::SimplePaint )
    {
        ReleaseDC(paintDc);
        ValidateRect(getHandle(), NULL);

        paintRect.left = 0;
        paintRect.top = 0;
        paintRect.right = 0;
        paintRect.bottom = 0;
        paintWidth = 0;
        paintHeight = 0;
        paintDc = NULL;
        paintStatus = PaintStatus::NotPainting;
    }
    else if ( paintStatus == PaintStatus::BufferedPaint )
    {
        ::BitBlt(paintFinalDc, paintRect.left, paintRect.top, paintWidth, paintHeight, paintDc, 0, 0, SRCCOPY);
        ::DeleteObject(paintMemBitmap);
        ::DeleteDC(paintDc);
        ReleaseDC(paintFinalDc);
        ValidateRect(getHandle(), NULL);

        paintRect.left = 0;
        paintRect.top = 0;
        paintRect.right = 0;
        paintRect.bottom = 0;
        paintWidth = 0;
        paintHeight = 0;
        paintDc = NULL;
        paintFinalDc = NULL;
        paintMemBitmap = NULL;
        paintStatus = PaintStatus::NotPainting;
    }
}

int WindowsItem::PaintWidth()
{
    return paintWidth;
}

int WindowsItem::PaintHeight()
{
    return paintHeight;
}

void WindowsItem::FillPaintArea(HBRUSH hBrush)
{
    if ( paintStatus != PaintStatus::NotPainting )
        ::FillRect(paintDc, &paintRect, hBrush);
}

void WindowsItem::FrameRect(HBRUSH hBrush, RECT &rect)
{
    if ( rect.left < rect.right || rect.top < rect.bottom )
        ::FrameRect(paintDc, &rect, hBrush);
}

int WindowsItem::GetWinTextLen()
{
    return ::GetWindowTextLength(getHandle());
}

std::string WindowsItem::GetWinText()
{
    std::string text("");
    WindowsItem::GetWinText(text);
    return text;
}

bool WindowsItem::GetWinText(std::string &outText)
{
    int titleLength = ::GetWindowTextLength(getHandle()) + 1;
    if ( titleLength > 1 )
    {
        std::unique_ptr<char> titleText(new char[titleLength]);
        if ( ::GetWindowText(getHandle(), titleText.get(), titleLength) )
        {
            titleText.get()[titleLength - 1] = '\0';
            outText = titleText.get();
            return true;
        }
    }
    else if ( titleLength == 1 )
    {
        outText = "";
        return true;
    }
    return false;
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

LONG WindowsItem::GetWinLong(int index)
{
    return ::GetWindowLong(getHandle(), index);
}

void WindowsItem::SetWinLong(int index, LONG newLong)
{
    ::SetWindowLong(getHandle(), index, newLong);
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
        ::SendMessage(getHandle(), WM_SETREDRAW, TRUE, 0);
    else
        ::SendMessage(getHandle(), WM_SETREDRAW, FALSE, 0);
}

void WindowsItem::RedrawThis()
{
    ::RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);
}

void WindowsItem::RefreshFrame()
{
    ::SetWindowPos(getHandle(), NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER | SWP_NOSIZE);
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

void WindowsItem::SetMedIcon(HANDLE hIcon)
{
    ::SendMessage(getHandle(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
}

bool WindowsItem::SetWinText(const std::string& newText)
{
    return ::SetWindowText(getHandle(), newText.c_str()) != 0;
}

bool WindowsItem::AddTooltip(const char* text)
{
    tooltipHandle = ::CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        getParent(), 0, ::GetModuleHandle(NULL), NULL);

    if ( tooltipHandle != NULL )
    {
        TOOLINFO toolInfo = {};
        toolInfo.cbSize = sizeof(TOOLINFO);
        toolInfo.hwnd = getParent();
        toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
        toolInfo.uId = (UINT_PTR)getHandle();
        toolInfo.lpszText = (LPSTR)text;
        if ( ::SendMessage(tooltipHandle, TTM_ADDTOOL, 0, (LPARAM)&toolInfo) == TRUE )
        {
            ::SendMessage(tooltipHandle, TTM_ACTIVATE, TRUE, 0);
            return true;
        }
        else
        {
            ::DestroyWindow(tooltipHandle);
            tooltipHandle = NULL;
        }
    }
    return false;
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

void WindowsItem::EnableThis()
{
    ::EnableWindow(getHandle(), TRUE);
}

void WindowsItem::DisableThis()
{
    ::EnableWindow(getHandle(), FALSE);
}

void WindowsItem::SetWidth(int newWidth)
{
    ::SetWindowPos(getHandle(), NULL, 0, 0, newWidth, Height(), SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}

void WindowsItem::SetHeight(int newHeight)
{
    ::SetWindowPos(getHandle(), NULL, 0, 0, Width(), newHeight, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}

void WindowsItem::SetSize(int newWidth, int newHeight)
{
    ::SetWindowPos(getHandle(), NULL, 0, 0, newWidth, newHeight, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}
