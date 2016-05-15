#ifndef LEFTBAR_H
#define LEFTBAR_H
#include "WindowsUI/WindowsUI.h"
#include "MainTree.h"
#include "MiniMap.h"

class LeftBar : public ClassWindow
{
    public:
        MiniMap miniMap;
        MainTree mainTree;

        bool CreateThis(HWND hParent);

    protected:
        virtual void NotifyTreeSelChanged(LPARAM newValue); // Sent when a new tree item is selected
        LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif