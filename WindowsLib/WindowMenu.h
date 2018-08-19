#ifndef WINDOWMENU_H
#define WINDOWMENU_H
#include "DataTypes.h"
#include "WinUIEnums.h"
#include <Windows.h>

namespace WinLib {

    class WindowMenu
    {
        public:
            WindowMenu();
            virtual ~WindowMenu();

            bool FindThis(HWND hParent);
            void SetCheck(u32 itemId, bool checked);
        
        private:
            HMENU hMenu;
    };

}

#endif