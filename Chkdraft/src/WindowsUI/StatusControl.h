#ifndef STATUSCONTROL_H
#define STATUSCONTROL_H
#include "WindowControl.h"

class StatusControl : public WindowControl
{
    public:
        virtual ~StatusControl();
        bool CreateThis(int numStatusWidths, int* statusWidths, DWORD dwExStyle, DWORD dwStyle, HWND hWndParent, HMENU hMenu);
        void AutoSize();
        void SetText(int index, const char* newText);

    private:

};

#endif