#ifndef MAINPLOT_H
#define MAINPLOT_H
#include "../../../WindowsLib/WindowsUI.h"
#include "../MainWindows/LoggerWindow.h"
#include "LeftBar.h"

class MainPlot : public WinLib::ClassWindow
{
    public:
        LeftBar leftBar;
        LoggerWindow loggerWindow;
        virtual ~MainPlot();
        bool CreateThis(HWND hParent, u64 id);

    protected:
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif