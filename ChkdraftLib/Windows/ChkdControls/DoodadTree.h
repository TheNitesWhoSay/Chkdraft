#ifndef DOODADTREE_H
#define DOODADTREE_H
#include "../../../WindowsLib/WindowsUi.h"
#include "../../CommonFiles/CommonFiles.h"

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