#include "StatusControl.h"

namespace WinLib {

    StatusControl::~StatusControl()
    {

    }

    bool StatusControl::CreateThis(int numStatusWidths, int* statusWidths, DWORD dwExStyle, DWORD dwStyle, HWND hWndParent, HMENU hMenu)
    {
        if ( WindowControl::CreateControl(dwExStyle, STATUSCLASSNAME, "", dwStyle, 0, 0, 0, 0, hWndParent, hMenu, false) )
        {
            if ( SendMessage(getHandle(), SB_SETPARTS, numStatusWidths, (LPARAM)statusWidths) )
                return true;
            else
                WindowControl::DestroyThis();
        }
        return false;
    }

    void StatusControl::AutoSize()
    {
        SendMessage(getHandle(), WM_SIZE, 0, 0);
    }

    void StatusControl::SetText(int index, const std::string & newText)
    {
        SendMessage(getHandle(), SB_SETTEXT, index, (INT_PTR)icux::toUistring(newText).c_str());
    }

}
