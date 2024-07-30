#ifndef STRINGPREVIEW_H
#define STRINGPREVIEW_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>

class StringPreviewWindow : public WinLib::ClassWindow
{
    public:
        virtual ~StringPreviewWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        bool DestroyThis();

    protected:
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif