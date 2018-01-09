#ifndef TEXTTRIG_H
#define TEXTTRIG_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"
#include "TextTrigGenerator.h"
#include "TextTrigCompiler.h"

class TextTrigWindow : public ClassDialog
{
    public:
        WindowMenu textTrigMenu;
        bool CreateThis(HWND hParent);
        void RefreshWindow();

    protected:
        void updateMenus();
        bool CompileEditText(ScenarioPtr map);
        virtual BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        virtual BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        EditControl editControl;
};

#endif