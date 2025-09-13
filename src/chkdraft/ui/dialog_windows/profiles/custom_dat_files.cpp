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
    
    for ( const auto & profile : chkd.profiles.profiles )
    {
        if ( !profile->remastered.cascPath.empty() && !usedDataFiles.contains(fixSystemPathSeparators(profile->remastered.cascPath)) )
            availableDataFiles.push_back(profile->remastered.cascPath);
        if ( !profile->classic.starDatPath.empty() && !usedDataFiles.contains(fixSystemPathSeparators(profile->classic.starDatPath)) )
            availableDataFiles.push_back(profile->classic.starDatPath);
        if ( !profile->classic.brooDatPath.empty() && !usedDataFiles.contains(fixSystemPathSeparators(profile->classic.brooDatPath)) )
            availableDataFiles.push_back(profile->classic.brooDatPath);
        if ( !profile->classic.patchRtPath.empty() && !usedDataFiles.contains(fixSystemPathSeparators(profile->classic.patchRtPath)) )
            availableDataFiles.push_back(profile->classic.patchRtPath);
    }

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

void CustomDatFilesWindow::MoveUpButtonClicked()
{
    if ( operateOnUsedFiles )
    {
        std::string selectedItem = listUsedDataFiles.GetSelectedItemText();
        if ( !selectedItem.empty() )
        {
            logger.info("TODO: Move used file \"" + selectedItem + "\" up");
        }
    }
}

void CustomDatFilesWindow::MoveDownButtonClicked()
{
    if ( operateOnUsedFiles )
    {
        std::string selectedItem = listUsedDataFiles.GetSelectedItemText();
        if ( !selectedItem.empty() )
        {
            logger.info("TODO: Move used file \"" + selectedItem + "\" down");
        }
    }
}

void CustomDatFilesWindow::MoveLeftButtonClicked()
{
    if ( !operateOnUsedFiles ) // Operating on available files
    {
        std::string selectedItem = listAvailableDataFiles.GetSelectedItemText();
        if ( !selectedItem.empty() )
        {
            logger.info("TODO: Move available file \"" + selectedItem + "\" to used files");
        }
    }
}

void CustomDatFilesWindow::MoveRightButtonClicked()
{
    if ( operateOnUsedFiles )
    {
        std::string selectedItem = listUsedDataFiles.GetSelectedItemText();
        if ( !selectedItem.empty() )
        {
            logger.info("TODO: Move used file \"" + selectedItem + "\" to available files");
        }
    }
}

void CustomDatFilesWindow::ToggleRelativePathClicked()
{
    if ( operateOnUsedFiles )
    {
        std::string selectedItem = listUsedDataFiles.GetSelectedItemText();
        if ( !selectedItem.empty() )
        {
            logger.info("TODO: Toggle file uses relative path \"" + selectedItem + "\"");
        }
    }
}

void CustomDatFilesWindow::BrowseClicked()
{
    logger.info("TODO: Browse...");
}

void CustomDatFilesWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
    case Id::ButtonMoveDataFileUp: MoveUpButtonClicked(); break;
    case Id::ButtonMoveDataFileDown: MoveDownButtonClicked(); break;
    case Id::ButtonUseDataFile: MoveLeftButtonClicked(); break;
    case Id::ButtonRemoveDataFile: MoveRightButtonClicked(); break;
    case Id::ButtonToggleRelativePath: ToggleRelativePathClicked(); break;
    case Id::ButtonBrowseDataFile: BrowseClicked(); break;
    }
}

LRESULT CustomDatFilesWindow::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    switch ( nmhdr->code )
    {
    case NM_SETFOCUS:
        if ( idFrom == Id::ListUsedDataFiles )
            operateOnUsedFiles = true;
        else if ( idFrom == Id::ListAvailableDataFiles )
            operateOnUsedFiles = false;
        break;
    }
    return ClassWindow::Notify(hWnd, idFrom, nmhdr);
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
