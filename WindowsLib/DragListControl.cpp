#include "DragListControl.h"
#include <CommCtrl.h>

namespace WinLib {

    DragListControl::~DragListControl()
    {

    }

    bool DragListControl::CreateThis(HWND hParent, int x, int y, int width, int height, u64 id)
    {
        if ( WindowControl::CreateControl( WS_EX_CLIENTEDGE, WC_LISTBOX, "", WS_CHILD|WS_TABSTOP|LBS_NOTIFY|LBS_SORT,
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

}
