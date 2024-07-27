#ifndef LOCATIONTREE_H
#define LOCATIONTREE_H
#include <windows/windows_ui.h>
#include <common_files/common_files.h>

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