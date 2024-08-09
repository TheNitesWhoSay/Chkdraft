#ifndef BRIEFINGTEMPLATES_H
#define BRIEFINGTEMPLATES_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>

class BriefingTemplatesWindow : public WinLib::ClassWindow
{
    public:
        virtual ~BriefingTemplatesWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        bool DestroyThis();
        void RefreshWindow();
        void DoSize();

    protected:
        void CreateSubWindows(HWND hWnd);

    private:
};

#endif