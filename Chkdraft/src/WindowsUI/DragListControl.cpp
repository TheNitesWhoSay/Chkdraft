#include "DragListControl.h"

bool DragListControl::CreateThis(HWND hParent, int x, int y, int width, int height, u32 id)
{
    if ( WindowControl::CreateControl( WS_EX_CLIENTEDGE, "LISTBOX", NULL, WS_CHILD|WS_TABSTOP|LBS_NOTIFY|LBS_SORT,
                                       x, y, width, height, hParent, (HMENU)id, false ) )
    {
        if ( MakeDragList(getHandle()) != 0 )
        {
            ShowWindow(getHandle(), SW_SHOWNORMAL);
            return true;
        }
        else
            WindowControl::DestroyThis();
    }
    return false;
}
