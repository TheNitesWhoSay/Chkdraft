#ifndef UNITTREE_H
#define UNITTREE_H
#include "WindowsUI/WindowsUI.h"

class UnitTree : public TreeViewControl
{
    public:
        bool AddThis(HWND hTree, HTREEITEM hRoot);
        bool CreateThis(HWND hParent, int x, int y, int width, int height, bool hasButtons, u32 id);

        void UpdateUnitNames(const char** displayNames);
        void UpdateUnitName(int UnitID);

    protected:
        void InsertAllUnits(HTREEITEM hParent);
        void InsertZergItems(HTREEITEM hParent);
        void InsertTerranItems(HTREEITEM hParent);
        void InsertProtossItems(HTREEITEM hParent);
        void InsertNeutralItems(HTREEITEM hParent);
        void InsertUndefinedItems(HTREEITEM hParent);

    private:
        HTREEITEM UnitItems[228];
        const char* UnitDisplayName[233];

        void InsertUnits(HTREEITEM hParent, const int* items, int amount);
};

#endif