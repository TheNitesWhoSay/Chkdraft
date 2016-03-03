#ifndef DRAGLISTCONTROL_H
#define DRAGLISTCONTROL_H
#include "WindowControl.h"

class DragListControl : public WindowControl
{
    public:
        bool CreateThis(HWND hParent, int x, int y, int width, int height, u32 id);
};

#endif