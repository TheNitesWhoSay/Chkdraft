#ifndef MAINPLOT_H
#define MAINPLOT_H
#include <windows/windows_ui.h>
#include "ui/main_windows/logger_window.h"
#include "left_bar.h"

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