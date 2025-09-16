#ifndef CUSTOMDATFILES_H
#define CUSTOMDATFILES_H
#include <chkdraft/mapping/chkd_profiles.h>
#include <common_files/common_files.h>
#include <windows/windows_ui.h>

class CustomDatFilesWindow : public WinLib::ClassWindow
{
public:
    CustomDatFilesWindow(std::string & editProfileName) : editProfileName(editProfileName) {}
    bool CreateThis(HWND hParent, u64 windowId);
    void DestroyThis();
    void RefreshWindow();

protected:
    void MoveUpButtonClicked();
    void MoveDownButtonClicked();
    void MoveLeftButtonClicked();
    void MoveRightButtonClicked();
    void ToggleRelativePathClicked();
    void BrowseClicked();
    void NotifyButtonClicked(int idFrom, HWND hWndFrom) override; // Sent when a button or checkbox is clicked
    LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr) override;
    LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    void CreateSubWindows(HWND hWnd);
    ChkdProfile & getEditProfile();
    std::string & editProfileName;
    
    WinLib::TextControl textUsedDataFiles;
    WinLib::ListViewControl listUsedDataFiles;
    WinLib::TextControl textAvailableDataFiles;
    WinLib::ListViewControl listAvailableDataFiles;
    WinLib::ButtonControl buttonUseDataFile;
    WinLib::ButtonControl buttonRemoveDataFile;
    WinLib::ButtonControl buttonMoveDataFileUp;
    WinLib::ButtonControl buttonMoveDataFileDown;
    WinLib::ButtonControl buttonBrowseDataFile;
    WinLib::ButtonControl buttonToggleRelativePath;
    bool operateOnUsedFiles = true; // Else operate on available files
};

#endif