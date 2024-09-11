#ifndef PLAYERDROPDOWN_H
#define PLAYERDROPDOWN_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>

class PlayerDropdown : public WinLib::DropdownControl
{
    public:
        virtual ~PlayerDropdown();
        bool CreateThis(HWND hParent, int x, int y, int width, int height, u32 id, bool eightPlayer);
        bool GetPlayerNum(u8 & dest); // Gets player number by selected index
        u8 GetPlayerBySelNum(); // Gets player number by selected index rather than edit text
};

#endif