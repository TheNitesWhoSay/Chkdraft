#ifndef PROFILEEDITORSETTINGS_H
#define PROFILEEDITORSETTINGS_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>

class ProfileEditorSettingsWindow : public WinLib::ClassWindow
{
public:
    bool CreateThis(HWND hParent, u64 windowId);
    void DestroyThis();
    void RefreshWindow();

protected:
    LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    void CreateSubWindows(HWND hWnd);
    
    WinLib::TextControl testText;
};

#endif