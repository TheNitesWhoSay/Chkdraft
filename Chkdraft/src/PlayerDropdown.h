#ifndef PLAYERDROPDOWN_H
#define PLAYERDROPDOWN_H
#include "CommonFiles/CommonFiles.h"
#include "WindowsUI/WindowsUI.h"

class PlayerDropdown : public DropdownControl
{
	public:
		bool CreateThis(HWND hParent, int x, int y, int width, int height, u32 id);
		bool GetPlayerNum(u8 &dest);
};

#endif