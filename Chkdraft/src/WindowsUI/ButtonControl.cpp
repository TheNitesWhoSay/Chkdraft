#include "ButtonControl.h"

bool ButtonControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, const char* initText, u32 id)
{
    return WindowControl::CreateControl( WS_EX_CLIENTEDGE, "BUTTON", initText, WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON,
                                         x, y, width, height, hParent, (HMENU)id, true );
}

LRESULT ButtonControl::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = WindowControl::ControlProc(hWnd, msg, wParam, lParam);

    if ( msg == WM_LBUTTONUP || msg == WM_LBUTTONDOWN )
        SendMessage(hWnd, BM_SETSTYLE, BS_PUSHBUTTON, TRUE); // Prevent Border Effect

    return result;
}

bool ButtonControl::SetText(const char* newText)
{
    return SetWindowText(getHandle(), newText) != 0;
}
