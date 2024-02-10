#ifndef ISOMTREE_H
#define ISOMTREE_H
#include "../../../WindowsLib/WindowsUi.h"
#include "../../CommonFiles/CommonFiles.h"

class IsomTree : public WinLib::TreeViewControl
{
    public:
        virtual ~IsomTree();
        bool AddThis(HWND hTree, HTREEITEM hParent);
        void UpdateIsomTree();

    private:
        HTREEITEM hIsomRoot;
};

#endif