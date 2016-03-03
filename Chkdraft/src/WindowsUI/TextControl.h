#ifndef TEXTCONTROL_H
#define TEXTCONTROL_H
#include "WindowControl.h"

class TextControl : public WindowControl
{
    public:
        // Attempts to create a text control
        bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, const char* initText, u32 id);

        bool SetText(const char* newText);
};

#endif