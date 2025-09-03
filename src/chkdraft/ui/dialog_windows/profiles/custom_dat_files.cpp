#include "custom_dat_files.h"
#include <chkdraft.h>

enum_t(Id, u32, {
    TextUsedDataFiles = ID_FIRST,
    ListUsedDataFiles,
    TextAvailableDataFiles,
    ListAvailableDataFiles,
    ButtonUseDataFile,
    ButtonRemoveDataFile,
    ButtonMoveDataFileUp,
    ButtonMoveDataFileDown,
    ButtonForgetDataFile,
    TextDataFilePath,
    EditDataFilePath,
    ButtonBrowseDataFile,
    ButtonToggleRelativePath
});

bool CustomDatFilesWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "Custom Dat Files", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "Custom Dat Files", WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

void CustomDatFilesWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
}

void CustomDatFilesWindow::RefreshWindow()
{
    auto & editProfile = getEditProfile();

    std::vector<std::string> availableDataFiles {};
    std::set<std::string> usedDataFiles {};
    listUsedDataFiles.DeleteAllItems();
    if ( editProfile.useRemastered )
    {
        for ( const auto & datFilePath : editProfile.remastered.dataFiles )
            listUsedDataFiles.AddRow();

        int row = 0;
        for ( const auto & datFilePath : editProfile.remastered.dataFiles )
        {
            listUsedDataFiles.SetItemText(row, 0, datFilePath);
            usedDataFiles.insert(fixSystemPathSeparators(datFilePath));
            ++row;
        }
    }
    else
    {
        for ( const auto & datFilePath : editProfile.classic.dataFiles )
            listUsedDataFiles.AddRow();

        int row = 0;
        for ( const auto & datFilePath : editProfile.classic.dataFiles )
        {
            listUsedDataFiles.SetItemText(row, 0, datFilePath);
            usedDataFiles.insert(fixSystemPathSeparators(datFilePath));
            ++row;
        }
    }
    
    if ( !editProfile.remastered.cascPath.empty() && !usedDataFiles.contains(fixSystemPathSeparators(editProfile.remastered.cascPath)) )
        availableDataFiles.push_back(editProfile.remastered.cascPath);
    if ( !editProfile.classic.starDatPath.empty() && !usedDataFiles.contains(fixSystemPathSeparators(editProfile.classic.starDatPath)) )
        availableDataFiles.push_back(editProfile.classic.starDatPath);
    if ( !editProfile.classic.brooDatPath.empty() && !usedDataFiles.contains(fixSystemPathSeparators(editProfile.classic.brooDatPath)) )
        availableDataFiles.push_back(editProfile.classic.brooDatPath);
    if ( !editProfile.classic.patchRtPath.empty() && !usedDataFiles.contains(fixSystemPathSeparators(editProfile.classic.patchRtPath)) )
        availableDataFiles.push_back(editProfile.classic.patchRtPath);

    listAvailableDataFiles.DeleteAllItems();
    for ( auto & availableDataFile : availableDataFiles )
        listAvailableDataFiles.AddRow();

    int row = 0;
    for ( auto & availableDataFile : availableDataFiles )
    {
        listAvailableDataFiles.SetItemText(row, 0, availableDataFile);
        ++row;
    }
}

LRESULT CustomDatFilesWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}

void CustomDatFilesWindow::CreateSubWindows(HWND hWnd)
{
    textUsedDataFiles.CreateThis(hWnd, 5, 5, 150, 20, "In Use:", Id::TextUsedDataFiles);
    listUsedDataFiles.CreateThis(hWnd, 5, 20, 250, 270, false, false, Id::ListUsedDataFiles, true);
    listUsedDataFiles.Show();

    textAvailableDataFiles.CreateThis(hWnd, 290, 5, 150, 20, "Available:", Id::TextAvailableDataFiles);
    listAvailableDataFiles.CreateThis(hWnd, 290, 20, 250, 270, false, false, Id::ListAvailableDataFiles, true);
    listAvailableDataFiles.Show();

    buttonMoveDataFileUp.CreateThis(hWnd, 262, 40, 20, 20, "^", Id::ButtonMoveDataFileUp, false, false);
    buttonMoveDataFileDown.CreateThis(hWnd, 262, 70, 20, 20, "v", Id::ButtonMoveDataFileDown, false, false);
    buttonUseDataFile.CreateThis(hWnd, 262, 170, 20, 20, "<", Id::ButtonUseDataFile, false, false);
    buttonRemoveDataFile.CreateThis(hWnd, 262, 200, 20, 20, ">", Id::ButtonRemoveDataFile, false, false);
    
    buttonToggleRelativePath.CreateThis(hWnd, 5, 295, 120, 20, "Toggle Relative Path", Id::ButtonToggleRelativePath, false, false);
    buttonBrowseDataFile.CreateThis(hWnd, 130, 295, 60, 20, "Browse...", Id::ButtonBrowseDataFile, false, false);

    listUsedDataFiles.AddColumn(0, "", 500, 0);
    listAvailableDataFiles.AddColumn(0, "", 500, 0);
}

ChkdProfile & CustomDatFilesWindow::getEditProfile()
{
    for ( auto & profile : chkd.profiles.profiles )
    {
        if ( this->editProfileName == profile->profileName )
        {
            return *(profile);
        }
    }
    this->editProfileName = chkd.profiles().profileName;
    return chkd.profiles();
}
