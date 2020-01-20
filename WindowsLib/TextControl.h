#ifndef TEXTCONTROL_H
#define TEXTCONTROL_H
#include "WindowControl.h"

namespace WinLib {

    class TextControl : public WindowControl
    {
        public:
            virtual ~TextControl();

            // Attempts to create a text control
            bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, const std::string & initText, u64 id);

            bool SetText(const std::string & newText);
    };

}

#endif