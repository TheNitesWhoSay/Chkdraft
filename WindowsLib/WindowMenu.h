#ifndef WINDOWMENU_H
#define WINDOWMENU_H
#include "DataTypes.h"
#include "WinUIEnums.h"
#include <Windows.h>
#include <string>

namespace WinLib {

    class WindowMenu
    {
        public:
            WindowMenu();
            virtual ~WindowMenu();

            bool FindThis(HWND hParent);
            void SetCheck(u32 itemId, bool checked);
            void SetText(u32 itemId, const std::string & text);
        
        private:
            HMENU hMenu;
    };

}

#endif