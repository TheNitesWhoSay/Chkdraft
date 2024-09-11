#ifndef TEXTTRIG_H
#define TEXTTRIG_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>

class TextTrigWindow : public WinLib::ClassDialog
{
    public:
        WinLib::WindowMenu textTrigMenu;
        virtual ~TextTrigWindow();
        bool CreateThis(HWND hParent);
        void RefreshWindow();

    protected:
        void updateMenus();
        bool CompileEditText(Scenario & map);
        virtual BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        virtual BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        WinLib::EditControl editControl;
};

#endif