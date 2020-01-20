#ifndef CHECKBOXCONTROL_H
#define CHECKBOXCONTROL_H
#include "WindowControl.h"

namespace WinLib {

    class CheckBoxControl : public WindowControl
    {
        public:
            virtual ~CheckBoxControl();
            bool CreateThis(HWND hParent, int x, int y, int width, int height, bool checked, const std::string & text, u64 id);
            bool CreateThis(HWND hParent, int x, int y, int width, int height, bool checked, bool autoCheck, const std::string & text, u64 id);

            bool isChecked();
            void SetCheck(bool isChecked);
    };

}

#endif