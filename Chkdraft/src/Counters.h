#ifndef COUNTERS_H
#define COUNTERS_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"

class CountersWindow : public ClassWindow
{
    public:
        bool CreateThis(HWND hParent, u32 windowId);
        bool DestroyThis();
        void RefreshWindow();
        void DoSize();

    protected:
        void CreateSubWindows(HWND hWnd);

    private:
};

#endif