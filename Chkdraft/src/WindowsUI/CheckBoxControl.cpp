#include "CheckBoxControl.h"

bool CheckBoxControl::CreateThis(HWND hParent, int x, int y, int width, int height, bool checked, const char* text, u32 id)
{
    if ( WindowControl::CreateControl( 0, "BUTTON", text, WS_VISIBLE|WS_CHILD|BS_AUTOCHECKBOX|BS_VCENTER,
                                         x, y, width, height, hParent, (HMENU)id, false ) )
    {
        if ( checked )
            SendMessage(getHandle(), BM_SETCHECK, BST_CHECKED, 0);

        return true;
    }
    else
        return false;
}

bool CheckBoxControl::CreateThis(HWND hParent, int x, int y, int width, int height, bool checked, bool autoCheck, const char* text, u32 id)
{
    DWORD style = WS_VISIBLE | WS_CHILD | BS_VCENTER;
    if ( autoCheck )
        style |= BS_AUTOCHECKBOX;
    else
        style |= BS_CHECKBOX;

    if ( WindowControl::CreateControl(0, "BUTTON", text, style,
        x, y, width, height, hParent, (HMENU)id, false) )
    {
        if ( checked )
            SendMessage(getHandle(), BM_SETCHECK, BST_CHECKED, 0);

        return true;
    }
    else
        return false;
}

bool CheckBoxControl::isChecked()
{
    return SendMessage(getHandle(), BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void CheckBoxControl::SetCheck(bool isChecked)
{
    if ( isChecked )
        SendMessage(getHandle(), BM_SETCHECK, BST_CHECKED, 0);
    else
        SendMessage(getHandle(), BM_SETCHECK, BST_UNCHECKED, 0);
}
