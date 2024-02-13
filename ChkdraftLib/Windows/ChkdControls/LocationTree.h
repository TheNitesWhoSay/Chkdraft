#ifndef LOCATIONTREE_H
#define LOCATIONTREE_H
#include "../../../WindowsLib/WindowsUi.h"
#include "../../CommonFiles/CommonFiles.h"

class LocationTree : public WinLib::TreeViewControl
{
    public:
        virtual ~LocationTree();
        bool AddThis(HWND hTree, HTREEITEM hParent);
        HTREEITEM InsertLocationItem(const std::string & text, u32 index);
        void RebuildLocationTree(bool updateSelection = false);

    private:
        HTREEITEM hLocationRoot;
};

#endif