#ifndef COUNTERS_H
#define COUNTERS_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>

class CountersWindow : public WinLib::ClassWindow
{
    public:
        virtual ~CountersWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        bool DestroyThis();
        void RefreshWindow();
        void DoSize();

    protected:
        void CreateSubWindows(HWND hWnd);

    private:
};

#endif