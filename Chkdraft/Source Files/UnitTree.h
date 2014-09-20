#ifndef UNITTREE_H
#define UNITTREE_H
#include "Windows UI/WindowsUI.h"

class UnitTree : public TreeViewControl
{
	public:
		bool CreateThis(HWND hParent);
		bool CreateThis(TreeViewControl parentTree);
};

#endif