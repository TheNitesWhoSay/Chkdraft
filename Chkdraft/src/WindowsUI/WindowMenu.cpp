#include "WindowMenu.h"

WindowMenu::WindowMenu() : hMenu(NULL)
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
