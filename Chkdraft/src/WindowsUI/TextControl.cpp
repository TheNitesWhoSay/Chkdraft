#include "TextControl.h"

bool TextControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, const char* initText, u32 id)
{
    return WindowControl::CreateControl(0, "STATIC", initText, WS_VISIBLE|WS_CHILD, x, y, width, height, hParent, (HMENU)id, false);
}

bool TextControl::SetText(const char* newText)
{
    return Static_SetText(getHandle(), newText) != 0;
}
