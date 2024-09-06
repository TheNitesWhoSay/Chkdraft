#ifndef LIT_H
#define LIT_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>
#include <string>

class LitWindow : public WinLib::ClassDialog
{
    public:
        virtual ~LitWindow();
        bool CreateThis(HWND hParent);
        void RefreshWindow();

    protected:
        bool GetLitPaths(std::string & litDirectory, std::string & litPath);
        std::optional<std::string> WriteLitBat(const std::string & inputFilePath, const std::string & litDirectory,
            const std::string & textOutName, const std::string & trigOutName);
        bool RunLit(Scenario & map);
        void ButtonLit();
        void ButtonLitSave();
        virtual BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        virtual BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        WinLib::EditControl editPath;
};

#endif