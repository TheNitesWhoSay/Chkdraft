#ifndef LEFTBAR_H
#define LEFTBAR_H
#include <windows/windows_ui.h>
#include "ui/chkd_controls/main_tree.h"
#include "ui/chkd_controls/history_tree.h"
#include "mini_map.h"

class LeftBar : public WinLib::ClassWindow
{
    public:
        bool blockSelections = true;
        MiniMap miniMap;
        MainTree mainTree;
        HistoryTree historyTree;

        virtual ~LeftBar();
        bool CreateThis(HWND hParent);

    protected:
        virtual void NotifyTreeItemSelected(LPARAM newValue); // Sent when a new tree item is selected
        LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        float historyTreeSize = 0.25;
};

#endif