#ifndef BUTTONCONTROL_H
#define BUTTONCONTROL_H
#include "WindowControl.h"

class ButtonControl : public WindowControl
{
    public:
        bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, const char* initText, u32 id); // Attempts to create a button
        LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        bool SetText(const char* newText); // Sets new text content
};

#endif