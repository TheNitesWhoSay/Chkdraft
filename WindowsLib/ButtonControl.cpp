#include "ButtonControl.h"
#include <SimpleIcu.h>

namespace WinLib {

    ButtonControl::ButtonControl() : image(NULL)
    {
    
    }

    ButtonControl::~ButtonControl()
    {
        if ( image != NULL )
            ::DeleteObject(image);
    }

    bool ButtonControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, const std::string & initText, u64 id, bool imageButton)
    {
        if ( imageButton )
        {
            return WindowControl::CreateControl( NULL, WC_BUTTON, initText, WS_VISIBLE|WS_CHILD|BS_BITMAP,
                                                 x, y, width, height, hParent, (HMENU)id, true );
        }
        else
        {
            return WindowControl::CreateControl( WS_EX_CLIENTEDGE, WC_BUTTON, initText, WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON,
                                                 x, y, width, height, hParent, (HMENU)id, true );
        }
    }

    LRESULT ButtonControl::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        LRESULT result = WindowControl::ControlProc(hWnd, msg, wParam, lParam);

        if ( msg == WM_LBUTTONUP || msg == WM_LBUTTONDOWN )
            SendMessage(hWnd, BM_SETSTYLE, BS_PUSHBUTTON, TRUE); // Prevent Border Effect

        return result;
    }

    bool ButtonControl::SetText(const std::string & newText)
    {
        return WindowsItem::SetWinText(newText);
    }
    
    bool ButtonControl::SetImageFromResourceId(u64 resourceId)
    {
        image = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(resourceId));
        if ( image != NULL )
        {
            SendMessage(getHandle(), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)image);
            return true;
        }
        return false;
    }

}
