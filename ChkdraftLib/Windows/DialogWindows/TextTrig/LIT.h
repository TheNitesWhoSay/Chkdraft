#ifndef LIT_H
#define LIT_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"
#include <string>

class LitWindow : public WinLib::ClassDialog
{
    public:
        virtual ~LitWindow();
        bool CreateThis(HWND hParent);
        void RefreshWindow();

    protected:
        bool GetLitPaths(std::string & litDirectory, std::string & litPath);
        bool GetInputPaths(std::string & luaDirectory, std::string & luaName);
        bool WriteLitBat(std::string & luaDirectory, std::string & luaName, std::string & litDirectory, std::string & litBatPath,
            std::string & textOutName, std::string & trigOutName);
        bool RunLit(ScenarioPtr map);
        void ButtonLit();
        void ButtonLitSave();
        virtual BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        virtual BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        WinLib::EditControl editPath;
};

#endif