#ifndef RADIOBUTTONCONTROL_H
#define RADIOBUTTONCONTROL_H
#include "WindowControl.h"

class RadioButtonControl : public WindowControl
{
    public:
        bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, const char* initText, u32 id);
        void SetCheck(bool isChecked);
};

#endif