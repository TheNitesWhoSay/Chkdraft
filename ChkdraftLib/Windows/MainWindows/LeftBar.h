#ifndef LEFTBAR_H
#define LEFTBAR_H
#include "../../../WindowsLib/WindowsUI.h"
#include "../ChkdControls/MainTree.h"
#include "MiniMap.h"

class LeftBar : public WinLib::ClassWindow
{
    public:
        MiniMap miniMap;
        MainTree mainTree;

        virtual ~LeftBar();
        bool CreateThis(HWND hParent);

    protected:
        virtual void NotifyTreeItemSelected(LPARAM newValue); // Sent when a new tree item is selected
        LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif