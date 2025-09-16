#include "input_dialog.h"
#include <common_files/common_files.h>

enum_t(Id, u32, {
    DialogInputString = IDD_DIALOG_INPUTSTRING,
    TextPrompt = IDC_STATIC_PROMPT,
    EditInput = IDC_EDIT1,
    ButtonOk = IDOK,
    ButtonCancel = IDCANCEL
});

std::optional<std::string> InputDialog::getInput(HWND hParent, const std::string & windowTitle, const std::string & prompt, const std::string & initText)
{
    InputDialog inputDialog {};
    inputDialog.windowTitle = windowTitle;
    inputDialog.promptText = prompt;
    inputDialog.initText = initText;
    inputDialog.CreateDialogBox(MAKEINTRESOURCE(Id::DialogInputString), hParent);
    return inputDialog.resultText;
}

void InputDialog::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
    case Id::ButtonOk:
        this->resultText = std::optional<std::string>(editInput.GetWinText());
        EndDialog(getHandle(), 0);
        break;
    case Id::ButtonCancel:
        this->resultText = std::nullopt;
        EndDialog(getHandle(), 0);
        break;
    }
}

BOOL InputDialog::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
    case WM_INITDIALOG:
        ClassDialog::SetWinText(windowTitle);
        textPrompt.FindThis(hWnd, Id::TextPrompt);
        textPrompt.SetWinText(promptText);
        editInput.FindThis(hWnd, Id::EditInput);
        editInput.SetWinText(initText);
        editInput.SelectAll();
        editInput.FocusThis();
        break;
    case WM_CLOSE:
        this->resultText = std::nullopt;
        EndDialog(getHandle(), 0);
        break;
    }
    return ClassDialog::DlgProc(hWnd, msg, wParam, lParam);
}
