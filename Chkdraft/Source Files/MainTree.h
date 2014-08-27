#ifndef MAINTREE_H
#define MAINTREE_H
#include "Windows UI/WindowsUI.h"
#include "Mapping Core/MappingCore.h"
#include "GuiAccel.h"

class MainTree : public TreeViewControl
{
	public:
		void InsertLocationItem(const char* text, u32 index);
		void BuildLocationTree();
		void BuildMainTree();
		void UpdateTreeItemText(HWND hWnd, int UnitID);
		void ChangeDisplayName(HWND hWnd, int UnitID, const char* name);
		TV_INSERTSTRUCT InsertUnits(HWND hWnd, TV_INSERTSTRUCT tvinsert, const int* items, int amount);
		HTREEITEM InsertUnitGrouping(HWND hWnd, TV_INSERTSTRUCT tvinsert, const char* pszText, HTREEITEM LastGroup);
		TV_INSERTSTRUCT InsertItems(HWND hWnd, TV_INSERTSTRUCT tvinsert, const char** items, int amount);

	private:
		TreeView DefaultTree;
		HTREEITEM hLocationTreeRoot;
		TV_INSERTSTRUCT LocationTree;
		TV_INSERTSTRUCT doodads;
};

#endif