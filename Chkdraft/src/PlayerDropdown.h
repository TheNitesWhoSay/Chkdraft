#ifndef PLAYERDROPDOWN_H
#define PLAYERDROPDOWN_H
#include "Common Files/CommonFiles.h"
#include "Windows UI/WindowsUI.h"

class PlayerDropdown : public DropdownControl
{
	public:
		bool CreateThis(HWND hParent, int x, int y, int width, int height, u32 id);
		bool GetPlayerNum(u8 &dest);
};

#endif