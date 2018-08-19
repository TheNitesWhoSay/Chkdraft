#include "GroupBoxControl.h"
#include <SimpleIcu.h>

namespace WinLib {

    GroupBoxControl::~GroupBoxControl()
    {

    }

    bool GroupBoxControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, const std::string &initText, u32 id)
    {
        return WindowControl::CreateControl( 0, WC_BUTTON, initText, WS_CHILD|WS_VISIBLE|BS_GROUPBOX,
                                             x, y, width, height, hParent, (HMENU)id, false );
    }

    bool GroupBoxControl::SetText(const std::string& newText)
    {
        std::wstring wNewText = icux::toUtf16(newText);
        return SetWindowText(getHandle(), wNewText.c_str()) == TRUE;
    }

}
