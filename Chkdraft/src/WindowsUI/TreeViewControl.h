#ifndef TREEVIEWCONTROL_H
#define TREEVIEWCONTROL_H
#include "WindowControl.h"

class TreeViewControl : public WindowControl
{
    public:
        bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool hasButtons, u32 id);
        HTREEITEM InsertTreeItem(HTREEITEM hParentItem, const char* text, LPARAM lParam);
        bool SetHandle(HWND hWnd);
        bool SetItemText(HTREEITEM hItem, const char* newText);
        bool ExpandItem(HTREEITEM hItem);
        void EmptySubTree(HTREEITEM hRoot);

    protected:
        LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        HTREEITEM InsertParent(const char* text, LPARAM lParam);
};

#endif