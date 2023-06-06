#include "WindowMenu.h"
#include "../CrossCutLib/SimpleIcu.h"

namespace WinLib {

    WindowMenu::WindowMenu() : hMenu(NULL)
    {

    }

    WindowMenu::~WindowMenu()
    {

    }

    bool WindowMenu::FindThis(HWND hParent)
    {
        hMenu = GetMenu(hParent);
        return hMenu != NULL;
    }

    void WindowMenu::SetCheck(u32 itemId, bool checked)
    {
        if ( checked )
            CheckMenuItem(hMenu, itemId, MF_CHECKED);
        else
            CheckMenuItem(hMenu, itemId, MF_UNCHECKED);
    }
    
    void WindowMenu::SetText(u32 itemId, const std::string & text)
    {
        MENUITEMINFO menuItemInfo = {};
        menuItemInfo.cbSize = sizeof(MENUITEMINFO);
        menuItemInfo.fMask = MIIM_ID;
        if ( GetMenuItemInfo(hMenu, itemId, FALSE, &menuItemInfo) != 0 )
        {
            auto uiText = icux::toUistring(text);
            menuItemInfo.fMask = MIIM_STRING;
            menuItemInfo.dwTypeData = (decltype(menuItemInfo.dwTypeData))uiText.c_str();
            SetMenuItemInfo(hMenu, itemId, FALSE, &menuItemInfo);
        }
        

    }

}
