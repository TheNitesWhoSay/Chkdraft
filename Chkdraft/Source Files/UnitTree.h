#ifndef UNITTREE_H
#define UNITTREE_H
#include "Windows UI/WindowsUI.h"

struct UnitTreeView
{
	int numItems;
	HTREEITEM TreePtr[228];
	int UnitID[228];
};

class UnitTree : public TreeViewControl
{
	public:
		UnitTree();
		bool CreateThis(HWND parentTree);
		bool CreateThis(TreeViewControl parentTree);

		void UpdateDisplayNames(const char** displayNames);

	protected:
		HTREEITEM InsertUnitGrouping(TV_INSERTSTRUCT tvinsert, const char* pszText, HTREEITEM LastGroup);
		void UpdateTreeUnitText(int UnitID);
		TV_INSERTSTRUCT InsertUnits(TV_INSERTSTRUCT tvinsert, const int* items, int amount);

	private:
		UnitTreeView DefaultTree;
		const char* UnitDisplayName[233];
};

#endif