#ifndef STATUSCONTROL_H
#define STATUSCONTROL_H
#include "WindowControl.h"

namespace WinLib {

    class StatusControl : public WindowControl
    {
        public:
            virtual ~StatusControl();
            bool CreateThis(int numStatusWidths, int* statusWidths, DWORD dwExStyle, DWORD dwStyle, HWND hWndParent, HMENU hMenu);
            void AutoSize();
            void SetText(int index, const std::string & newText);

        private:

    };

}

#endif