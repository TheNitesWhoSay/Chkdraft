#ifndef WINDOWMENU_H
#define WINDOWMENU_H
#include "data_types.h"
#include "win_ui_enums.h"
#include <Windows.h>
#include <string>

namespace WinLib {

    class WindowMenu
    {
        public:
            WindowMenu();
            virtual ~WindowMenu();

            HMENU getHandle();
            bool FindThis(HWND hParent);
            void SetCheck(u32 itemId, bool checked);
            void SetText(u32 itemId, const std::string & text);
        
        private:
            HMENU hMenu;
    };

}

#endif