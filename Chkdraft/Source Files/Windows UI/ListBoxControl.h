#ifndef LISTBOXCONTROL_H
#define LISTBOXCONTROL_H
#include "WindowControl.h"

class ListBoxControl : public WindowControl
{
	public:
		// Attempts to create a list box
		bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool ownerDrawn, bool multiColumn, u32 id);
		void ClearItems();
		int AddString(const char* str);
		int GetNumItems();
		int ItemHeight();
		int ColumnWidth();
		int GetNumSel();
		bool GetSelString(int n, std::string &str); // Attempts to get the nth selected item's string
};

#endif