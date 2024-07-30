#ifndef ISOMTREE_H
#define ISOMTREE_H
#include <windows/windows_ui.h>
#include <common_files/common_files.h>

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