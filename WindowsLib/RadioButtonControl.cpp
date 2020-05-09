#include "RadioButtonControl.h"

namespace WinLib {

    RadioButtonControl::~RadioButtonControl()
    {

    }

    bool RadioButtonControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, const std::string & initText, u64 id)
    {
        return WindowControl::CreateControl( 0, WC_BUTTON, initText, WS_VISIBLE|WS_CHILD|BS_AUTORADIOBUTTON,
                                             x, y, width, height, hParent, (HMENU)id, false );
    }

    void RadioButtonControl::SetCheck(bool isChecked)
    {
        if ( isChecked )
            SendMessage(getHandle(), BM_SETCHECK, BST_CHECKED, 0);
        else
            SendMessage(getHandle(), BM_SETCHECK, BST_UNCHECKED, 0);
    }

}
