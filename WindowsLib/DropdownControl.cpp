#include "DropdownControl.h"
#include <SimpleIcu.h>
#include <memory>

namespace WinLib {

    DropdownControl::~DropdownControl()
    {

    }

    bool DropdownControl::CreateThis(HWND hParent, int x, int y, int width, int height, bool editable, bool alwaysList,
        u64 id, const std::vector<std::string> & items, HFONT font)
    {
        DWORD style = WS_VISIBLE | WS_CHILD | WS_VSCROLL | CBS_AUTOHSCROLL | CBS_HASSTRINGS;

        if ( editable )
        {
            if ( alwaysList )
                style |= CBS_DROPDOWNLIST;
            else
                style |= CBS_DROPDOWN;
        }
        else
        {
            if ( alwaysList )
                style |= CBS_SIMPLE | CBS_DROPDOWNLIST;
            else
                style |= CBS_SIMPLE | CBS_DROPDOWN;
        }

        if ( WindowControl::CreateControl(0, WC_COMBOBOX, "", style, x, y, width, height, hParent, (HMENU)id, false) )
        {
            HWND hWnd = getHandle();
            SendMessage(hWnd, WM_SETFONT, (WPARAM)font, MAKELPARAM(TRUE, 0));
            for ( size_t i = 0; i < items.size(); i++ )
                SendMessage(hWnd, CB_ADDSTRING, 0, (LPARAM)icux::toUistring(items[i]).c_str());
            return true;
        }
        return false;
    }

    void DropdownControl::SetSel(int index)
    {
        SendMessage(getHandle(), CB_SETCURSEL, index, 0);
    }

    void DropdownControl::ClearEditSel()
    {
        PostMessage(getHandle(), CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));
    }

    int DropdownControl::GetSel()
    {
        return (int)SendMessage(getHandle(), CB_GETCURSEL, 0, 0);
    }

    bool DropdownControl::GetItemText(int index, std::string & dest)
    {
        LRESULT textLength = SendMessage(getHandle(), CB_GETLBTEXTLEN, index, 0);
        if ( textLength != CB_ERR )
        {
            std::unique_ptr<icux::codepoint> text(new icux::codepoint[textLength + 1]);
            if ( SendMessage(getHandle(), CB_GETLBTEXT, index, (LPARAM)text.get()) != CB_ERR )
            {
                dest = icux::toUtf8(text.get());
                return true;
            }
        }
        return false;
    }
    
    int DropdownControl::GetNumItems()
    {
        return int(SendMessage(getHandle(), CB_GETCOUNT, 0, 0));
    }

    void DropdownControl::ClearItems()
    {
        SendMessage(getHandle(), CB_RESETCONTENT, 0, 0);
    }

    int DropdownControl::AddItem(const std::string & item)
    {
        LRESULT result = SendMessage(getHandle(), CB_ADDSTRING, (WPARAM)NULL, (LPARAM)icux::toUistring(item).c_str());
        return int(result);// != CB_ERR && result != CB_ERRSPACE;
    }
    
    int DropdownControl::RemoveItem(int index)
    {
        return int(SendMessage(getHandle(), CB_DELETESTRING, WPARAM(index), NULL));
    }

}
