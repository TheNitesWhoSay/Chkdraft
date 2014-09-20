#ifndef MAINTREE_H
#define MAINTREE_H
#include "Windows UI/WindowsUI.h"
#include "Mapping Core/MappingCore.h"
#include "GuiAccel.h"

class MainTree : public TreeViewControl
{
	public:

		void BuildMainTree();

		void UpdateDisplayNames(const char** displayNames);
		void ChangeDisplayName(HWND hWnd, int UnitID, const char* name);

		void InsertLocationItem(const char* text, u32 index);
		void RebuildLocationTree();


	protected:

		TV_INSERTSTRUCT InsertItems(HWND hWnd, TV_INSERTSTRUCT tvinsert, const char** items, int amount);

		HTREEITEM InsertUnitGrouping(HWND hWnd, TV_INSERTSTRUCT tvinsert, const char* pszText, HTREEITEM LastGroup);
		void UpdateTreeUnitText(HWND hWnd, int UnitID);
		TV_INSERTSTRUCT InsertUnits(HWND hWnd, TV_INSERTSTRUCT tvinsert, const int* items, int amount);


	private:

		TreeView DefaultTree;
		HTREEITEM hLocationTreeRoot;
		TV_INSERTSTRUCT LocationTree;
		TV_INSERTSTRUCT doodads;
		const char* UnitDisplayName[233];
};

#endif