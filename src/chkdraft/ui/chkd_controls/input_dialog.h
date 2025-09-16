#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H
#include <windows/windows_ui.h>
#include <optional>
#include <string>

class InputDialog : public WinLib::ClassDialog
{
    public:
        static std::optional<std::string> getInput(HWND hParent, const std::string & windowTitle, const std::string & prompt, const std::string & initText);

    protected:
        void NotifyButtonClicked(int idFrom, HWND hWndFrom) override;
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

    private:
        std::string windowTitle {};
        std::string promptText {};
        std::string initText {};
        std::optional<std::string> resultText {};
        WinLib::TextControl textPrompt;
        WinLib::EditControl editInput;
};

#endif