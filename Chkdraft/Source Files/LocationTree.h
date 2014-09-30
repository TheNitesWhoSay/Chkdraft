#ifndef LOCATIONTREE_H
#define LOCATIONTREE_H
#include "Windows UI/WindowsUI.h"

class LocationTree : public TreeViewControl
{
	public:
		bool CreateThis(HWND hParent);
		void InsertLocationItem(const char* text, u32 index);
		void RebuildLocationTree();

	private:
		HTREEITEM hLocationTreeRoot;
		TV_INSERTSTRUCT locationInsert;
};

#endif