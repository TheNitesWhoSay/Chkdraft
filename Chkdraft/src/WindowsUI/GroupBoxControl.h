#ifndef GROUPBOXCONTROL_H
#define GROUPBOXCONTROL_H
#include "WindowControl.h"

class GroupBoxControl : public WindowControl
{
    public:
        bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, const char* initText, u32 id); // Attempts to create a group box
        bool SetText(const std::string& newText);
};

#endif