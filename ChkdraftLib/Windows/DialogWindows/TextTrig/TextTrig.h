#ifndef TEXTTRIG_H
#define TEXTTRIG_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"

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