#ifndef GROUPBOXCONTROL_H
#define GROUPBOXCONTROL_H
#include "WindowControl.h"

namespace WinLib {

    class GroupBoxControl : public WindowControl
    {
        public:
            virtual ~GroupBoxControl();
            bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, const std::string & initText, u64 id); // Attempts to create a group box
    };

}

#endif