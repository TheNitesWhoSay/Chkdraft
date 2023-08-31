#ifndef TREEVIEWCONTROL_H
#define TREEVIEWCONTROL_H
#include "WindowControl.h"
#include <CommCtrl.h>

namespace WinLib {

    class TreeViewControl : public WindowControl
    {
        public:
            virtual ~TreeViewControl();
            bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool hasButtons, u64 id);
            HTREEITEM InsertTreeItem(HTREEITEM hParentItem, const std::string & text, LPARAM lParam);
            bool SetHandle(HWND hWnd);
            bool SetItemText(HTREEITEM hItem, const std::string & newText);
            bool ExpandItem(HTREEITEM hItem);
            void EmptySubTree(HTREEITEM hRoot);
            void SelectItem(HTREEITEM hItem);
            std::optional<LPARAM> GetSelData();

        protected:
            LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        private:
            HTREEITEM InsertParent(const std::string & text, LPARAM lParam);
    };

}

#endif