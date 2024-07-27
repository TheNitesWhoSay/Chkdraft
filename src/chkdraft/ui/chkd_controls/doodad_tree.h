#ifndef DOODADTREE_H
#define DOODADTREE_H
#include <windows/windows_ui.h>
#include <common_files/common_files.h>

class DoodadTree : public WinLib::TreeViewControl
{
    public:
        virtual ~DoodadTree();
        bool AddThis(HWND hTree, HTREEITEM hParent);
        void UpdateDoodadTree();

    private:
        HTREEITEM hDoodadRoot;
};

#endif