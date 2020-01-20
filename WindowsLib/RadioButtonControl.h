#ifndef RADIOBUTTONCONTROL_H
#define RADIOBUTTONCONTROL_H
#include "WindowControl.h"

namespace WinLib {

    class RadioButtonControl : public WindowControl
    {
        public:
            virtual ~RadioButtonControl();
            bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, const std::string & initText, u64 id);
            void SetCheck(bool isChecked);
    };

}

#endif