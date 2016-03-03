#ifndef CHECKBOXCONTROL_H
#define CHECKBOXCONTROL_H
#include "WindowControl.h"

class CheckBoxControl : public WindowControl
{
    public:
        bool CreateThis(HWND hParent, int x, int y, int width, int height, bool checked, const char* text, u32 id);
        bool CreateThis(HWND hParent, int x, int y, int width, int height, bool checked, bool autoCheck, const char* text, u32 id);

        bool isChecked();
        void SetCheck(bool isChecked);
};

#endif