#include "TextControl.h"
#include <SimpleIcu.h>

namespace WinLib {

    TextControl::~TextControl()
    {

    }

    bool TextControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, const std::string & initText, u64 id)
    {
        return WindowControl::CreateControl(0, WC_STATIC, initText, WS_VISIBLE|WS_CHILD, x, y, width, height, hParent, (HMENU)id, false);
    }

    bool TextControl::SetText(const std::string & newText)
    {
        return Static_SetText(getHandle(), icux::toUistring(newText).c_str()) != 0;
    }

}
