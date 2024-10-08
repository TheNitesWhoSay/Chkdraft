#ifndef DRAGLISTCONTROL_H
#define DRAGLISTCONTROL_H
#include "window_control.h"

namespace WinLib {

    class DragListControl : public WindowControl
    {
        public:
            virtual ~DragListControl();
            bool CreateThis(HWND hParent, int x, int y, int width, int height, u64 id);
    };

}

#endif