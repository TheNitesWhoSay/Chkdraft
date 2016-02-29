#ifndef LOCATIONTREE_H
#define LOCATIONTREE_H
#include "WindowsUI/WindowsUI.h"

class LocationTree : public TreeViewControl
{
    public:
        bool AddThis(HWND hTree, HTREEITEM hParent);
        void InsertLocationItem(const char* text, u32 index);
        void RebuildLocationTree();

    private:
        HTREEITEM hLocationRoot;
};

#endif
