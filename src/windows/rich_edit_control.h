#ifndef RICHEDITCONTROL_H
#define RICHEDITCONTROL_H
#include "WindowControl.h"
#include <richedit.h>
#include <string>

namespace WinLib {

    class RichEditControl : public WindowControl
    {
        public:
            RichEditControl();
            virtual ~RichEditControl();
            
            bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool readOnly, bool scrollable, u64 id);
            void SetText(const std::string & text);
            void SetUiText(const icux::uistring & text);
            void ScrollBottom();
            void SetScrollMatcher(RichEditControl* scrollMatcher);
            void AlignMatcher();

        protected:
            virtual LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        private:
            bool scrollable;
            bool scrollTracking;
            bool hiddenScroll;
            RichEditControl* scrollMatcher;
            ::WPARAM scrollBottom;
            ::SETTEXTEX setTextSettings;
            ::SCROLLBARINFO scrollBarInfo;
            ::SCROLLINFO scrollInfo;
            ::POINT scrollPoint;
            struct LibraryLoader { LibraryLoader(); };
            static LibraryLoader libraryLoader;
    };

}

#endif