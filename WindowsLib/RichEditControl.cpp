#include "RichEditControl.h"
#include <Richedit.h>
#include <iostream>

namespace WinLib {

    RichEditControl::LibraryLoader RichEditControl::libraryLoader;

    RichEditControl::LibraryLoader::LibraryLoader()
    {
        LoadLibrary(TEXT("Msftedit.dll"));
    }

    RichEditControl::RichEditControl() : scrollBottom(MAKEWPARAM(SB_BOTTOM, 0)), scrollMatcher(nullptr), hiddenScroll(false), scrollable(false), scrollTracking(false)
    {
        scrollBarInfo = {};
        scrollBarInfo.cbSize = sizeof(::SCROLLBARINFO);
        scrollPoint.x = 0;
        scrollPoint.y = 0;
        scrollInfo = {};
        scrollInfo.cbSize = sizeof(::SCROLLINFO);
        scrollInfo.fMask = SIF_POS;

#ifdef UNICODE
        setTextSettings.flags = ST_NEWCHARS|ST_UNICODE;
        setTextSettings.codepage = 1200;
#else
        setTextSettings.flags = ST_NEWCHARS;
        setTextSettings.codepage = CP_ACP;
#endif
    }

    RichEditControl::~RichEditControl()
    {

    }
    
    bool RichEditControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool readOnly, bool scrollable, u64 id)
    {
        DWORD dwStyle = WS_VISIBLE|WS_CHILD|ES_MULTILINE;

        if ( readOnly )
            dwStyle |= ES_READONLY;
        else
            dwStyle |= ES_WANTRETURN;

        this->scrollable = scrollable;
        if ( scrollable )
            dwStyle |= ES_AUTOVSCROLL|ES_AUTOHSCROLL|WS_VSCROLL|WS_HSCROLL;

        return WindowControl::CreateControl( WS_EX_CLIENTEDGE, icux::toUistring(MSFTEDIT_CLASS), "", dwStyle,
                                             x, y, width, height,
                                             hParent, (HMENU)id, true );
    }

    void RichEditControl::SetText(const std::string & text)
    {
        auto displayText = icux::toUistring(text);
        ::SendMessage(getHandle(), EM_SETTEXTEX, (WPARAM)&setTextSettings, (LPARAM)displayText.c_str());
    }

    void RichEditControl::SetUiText(const icux::uistring & text)
    {
        ::SendMessage(getHandle(), EM_SETTEXTEX, (WPARAM)&setTextSettings, (LPARAM)text.c_str());
    }
    
    void RichEditControl::ScrollBottom()
    {
        ::SendMessage(getHandle(), WM_VSCROLL, scrollBottom, NULL);
    }
    
    void RichEditControl::SetScrollMatcher(RichEditControl* scrollMatcher)
    {
        this->scrollMatcher = scrollMatcher;
    }
    
    void RichEditControl::AlignMatcher()
    {
        ::GetScrollBarInfo(getHandle(), OBJID_VSCROLL, &scrollBarInfo);
        if ( scrollBarInfo.rgstate[0] == STATE_SYSTEM_INVISIBLE )
            scrollPoint.y = 0;
        else
        {
            ::GetScrollInfo(getHandle(), SB_VERT, &scrollInfo);
            scrollPoint.y = scrollInfo.nPos;
        }
        if ( scrollMatcher != nullptr )
            PostMessage(scrollMatcher->getHandle(), EM_SETSCROLLPOS, 0, (LPARAM)&scrollPoint);
    }

    LRESULT RichEditControl::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if ( msg == WM_MOUSEWHEEL && scrollable )
        {
            short units = (GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA)*3;
            for ( ; SendMessage(getHandle(), EM_GETSCROLLPOS, 0, (LPARAM)&scrollPoint) == 1 && scrollPoint.y > 0 && units > 0; --units )
                SendMessage(getHandle(), WM_VSCROLL, SB_LINEUP, NULL);
            for ( ; units < 0; ++units )
                SendMessage(getHandle(), WM_VSCROLL, SB_LINEDOWN, NULL);

            return 0;
        }
        else if ( scrollMatcher != nullptr )
        {
            if ( msg == WM_VSCROLL )
            {
                if ( LOWORD(wParam) == SB_THUMBTRACK )
                {
                    scrollTracking = true;
                    PostMessage(scrollMatcher->getHandle(), WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, HIWORD(wParam)), NULL);
                }
                else if ( LOWORD(wParam) == SB_THUMBPOSITION )
                    scrollTracking = false;
            }
            else if ( msg == WM_PAINT && !scrollTracking )
                AlignMatcher();
        }
        return WindowControl::ControlProc(hWnd, msg, wParam, lParam); // Take default action
    }

}