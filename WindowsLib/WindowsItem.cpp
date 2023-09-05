#include "WindowsItem.h"
#include "ResourceManager.h"
#include <SimpleIcu.h>
#include <memory>
#include <CommCtrl.h>
#include <iostream>

namespace WinLib {

    std::list<std::string> WindowsItem::registeredClasses; // Obligatory definition of static variable

    WindowsItem::WindowsItem() : windowsItemHandle(NULL), tooltipHandle(NULL)
    {

    }

    WindowsItem::~WindowsItem()
    {

    }

    void WindowsItem::DestroyThis()
    {
        if ( getHandle() != NULL )
            ::DestroyWindow(getHandle());
        windowsItemHandle = NULL;

        if ( tooltipHandle != NULL )
            ::DestroyWindow(tooltipHandle);
        tooltipHandle = NULL;
    }

    HWND WindowsItem::getHandle()
    {
        return windowsItemHandle;
    }

    HWND WindowsItem::getParent()
    {
        return ::GetParent(getHandle());
    }
    
    WinLib::DeviceContext WindowsItem::getDeviceContext()
    {
        if ( windowsItemHandle != NULL )
            return WinLib::DeviceContext(windowsItemHandle);
        else
            return WinLib::DeviceContext{};
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
        u64 menuResourceId, const std::string & className, HICON hIconSm, WNDPROC wndProc)
    {
        if ( getHandle() == NULL && windowClassName.length() <= 0 )
        {
            if ( WindowClassIsRegistered(className) )
            {
                try { windowClassName = className; }
                catch ( ... ) { return false; }
                return true;
            }
            else
            {
                icux::uistring sysClassName = icux::toUistring(className);
                WNDCLASSEX wc = {};
                wc.cbSize = sizeof(WNDCLASSEX);
                wc.style = style;
                wc.lpfnWndProc = wndProc;
                wc.hInstance = GetModuleHandle(NULL);
                wc.hIcon = hIcon;
                wc.hCursor = hCursor;
                wc.hbrBackground = hbrBackground;
                wc.lpszMenuName = MAKEINTRESOURCE(menuResourceId);
                wc.lpszClassName = sysClassName.c_str();
                wc.hIconSm = hIconSm;
                if ( RegisterClassEx(&wc) != 0 )
                {
                    try { windowClassName = className; registeredClasses.push_back(windowClassName); }
                    catch ( ... ) { return false; }
                    return true;
                }
            }
        }
        return false;
    }

    bool WindowsItem::WindowClassIsRegistered(const std::string & lpszClassName)
    {
        try { windowClassName = lpszClassName; }
        catch ( ... ) { return false; }
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

    std::string & WindowsItem::WindowClassName()
    {
        return windowClassName;
    }

    int WindowsItem::GetWinTextLen()
    {
        return ::GetWindowTextLength(getHandle());
    }

    std::optional<std::string> WindowsItem::GetWinText()
    {
        int titleLength = ::GetWindowTextLength(getHandle()) + 1;
        if ( titleLength > 1 )
        {
            std::unique_ptr<icux::codepoint> titleText(new icux::codepoint[titleLength]);
            if ( ::GetWindowText(getHandle(), titleText.get(), titleLength) )
            {
                titleText.get()[titleLength - 1] = '\0';
                return icux::toUtf8(icux::uistring(titleText.get(), size_t(titleLength)-1));
            }
        }
        else if ( titleLength == 1 )
            return "";

        return std::nullopt;
    }

    bool WindowsItem::getWindowRect(RECT & rect)
    {
        return ::GetWindowRect(getHandle(), &rect) != 0;
    }

    bool WindowsItem::getClientRect(RECT & rect)
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

    void WindowsItem::setFont(int width, int height, const std::string & fontName, bool redrawImmediately)
    {
        HFONT hFont = ResourceManager::getFont(width, height, fontName);
        if ( hFont != NULL )
            ::SendMessage(getHandle(), WM_SETFONT, (WPARAM)hFont, MAKELPARAM(redrawImmediately ? TRUE : FALSE, 0));
    }

    void WindowsItem::setFont(HFONT hFont, bool redrawImmediately)
    {
        ::SendMessage(getHandle(), WM_SETFONT, (WPARAM)hFont, MAKELPARAM(redrawImmediately ? TRUE : FALSE, 0));
    }

    void WindowsItem::setDefaultFont(bool redrawImmediately)
    {
        HFONT hFont = ResourceManager::getDefaultFont();
        if ( hFont != NULL )
            ::SendMessage(getHandle(), WM_SETFONT, (WPARAM)hFont, MAKELPARAM(redrawImmediately ? TRUE : FALSE, 0));
    }

    void WindowsItem::replaceChildFonts(int width, int height, const std::string & fontName)
    {
        HFONT hFont = ResourceManager::getFont(width, height, fontName);
        if ( hFont != NULL )
            ::EnumChildWindows(getHandle(), (WNDENUMPROC)WinLib::SetFont, (LPARAM)hFont);
    }

    void WindowsItem::replaceChildFonts(HFONT hFont)
    {
        if ( hFont != NULL )
            ::EnumChildWindows(getHandle(), (WNDENUMPROC)WinLib::SetFont, (LPARAM)hFont);
    }

    void WindowsItem::defaultChildFonts()
    {
        HFONT hFont = ResourceManager::getDefaultFont();
        if ( hFont != NULL )
            ::EnumChildWindows(getHandle(), (WNDENUMPROC)WinLib::SetFont, (LPARAM)hFont);
    }

    void WindowsItem::LockCursor()
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

    void WindowsItem::SetSmallIcon(HICON hIcon)
    {
        ::SendMessage(getHandle(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    }

    void WindowsItem::SetMedIcon(HICON hIcon)
    {
        ::SendMessage(getHandle(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    }

    bool WindowsItem::SetWinText(const std::string & newText)
    {
        return ::SetWindowText(getHandle(), icux::toUistring(newText).c_str()) != 0;
    }

    bool WindowsItem::AddTooltip(const std::string & text)
    {
        tooltipHandle = ::CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            getParent(), 0, ::GetModuleHandle(NULL), NULL);

        if ( tooltipHandle != NULL )
        {
            icux::uistring sysText = icux::toUistring(text);

            TOOLINFO toolInfo = {};
            toolInfo.cbSize = sizeof(TOOLINFO);
            toolInfo.hwnd = getParent();
            toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
            toolInfo.uId = (UINT_PTR)getHandle();
            toolInfo.lpszText = (LPTSTR)sysText.c_str();
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

}
