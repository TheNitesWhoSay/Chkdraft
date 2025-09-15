#include "custom_dat_files.h"
#include <chkdraft.h>
#include <filesystem>

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
            auto & editProfile = getEditProfile();
            auto & dataFiles = editProfile.useRemastered ? editProfile.remastered.dataFiles : editProfile.classic.dataFiles;

            int dataFileIndex = -1;
            for ( std::size_t i=0; i<dataFiles.size(); ++i )
            {
                if ( selectedItem == dataFiles[i] )
                {
                    dataFileIndex = int(i);
                    break;
                }
            }
            if ( dataFileIndex > 0 ) // Can't move index 0 or not found (-1) up
            {
                std::swap(dataFiles[dataFileIndex], dataFiles[dataFileIndex-1]);
                editProfile.saveProfile();
                RefreshWindow();
                listUsedDataFiles.SelectRow(dataFileIndex-1);
            }
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
            auto & editProfile = getEditProfile();
            auto & dataFiles = editProfile.useRemastered ? editProfile.remastered.dataFiles : editProfile.classic.dataFiles;

            std::size_t dataFileIndex = dataFiles.size();
            for ( std::size_t i=0; i<dataFiles.size(); ++i )
            {
                if ( selectedItem == dataFiles[i] )
                {
                    dataFileIndex = i;
                    break;
                }
            }
            if ( dataFileIndex < dataFiles.size()-1 ) // Can't move last entry or invalid entry (dataFiles.size()) down
            {
                std::swap(dataFiles[dataFileIndex], dataFiles[dataFileIndex+1]);
                editProfile.saveProfile();
                RefreshWindow();
                listUsedDataFiles.SelectRow(dataFileIndex+1);
            }
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
            auto & editProfile = getEditProfile();
            auto & dataFiles = editProfile.useRemastered ? editProfile.remastered.dataFiles : editProfile.classic.dataFiles;
            dataFiles.push_back(selectedItem);
            editProfile.saveProfile();
            operateOnUsedFiles = true;
            RefreshWindow();
            listUsedDataFiles.SelectRow(listUsedDataFiles.GetNumRows()-1);
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
            auto & editProfile = getEditProfile();
            auto & dataFiles = editProfile.useRemastered ? editProfile.remastered.dataFiles : editProfile.classic.dataFiles;
            for ( std::size_t i=0; i<dataFiles.size(); ++i )
            {
                if ( dataFiles[i] == selectedItem )
                {
                    dataFiles.erase(std::next(dataFiles.begin(), i));
                    editProfile.saveProfile();
                    operateOnUsedFiles = false;
                    RefreshWindow();
                    listAvailableDataFiles.SelectRowByText(selectedItem);
                    return;
                }
            }
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
            auto & editProfile = getEditProfile();
            auto & dataFiles = editProfile.useRemastered ? editProfile.remastered.dataFiles : editProfile.classic.dataFiles;
            auto profileDirectory = std::filesystem::path(getSystemFileDirectory(editProfile.profilePath, true));
            auto dataFilePath = std::filesystem::path(selectedItem);
            std::filesystem::current_path(profileDirectory);
            auto result = dataFilePath.is_relative() ? std::filesystem::canonical(dataFilePath) :
                std::filesystem::proximate(dataFilePath, profileDirectory).string();

            int sel = listUsedDataFiles.GetNextSelection();
            if ( sel != -1 && sel < dataFiles.size() )
            {
                dataFiles[sel] = result.string();
                editProfile.saveProfile();
                RefreshWindow();
                listUsedDataFiles.SelectRow(sel);
            }
        }
    }
}

void CustomDatFilesWindow::BrowseClicked()
{
    // Setup a filter for "All StarCraft Data Files" followed by MPQ files, followed by CASC files
    auto browser = NoPromptFileBrowser<u32>(Sc::DataFile::getDatFileFilter(), "");
    std::string datFilePath {};
    u32 filterIndex = 0;
    if ( browser.browseForOpenPath(datFilePath, filterIndex) )
    {
        auto & editProfile = getEditProfile();
        auto & dataFiles = editProfile.useRemastered ? editProfile.remastered.dataFiles : editProfile.classic.dataFiles;
        if ( filterIndex == 0 ) // MPQ
        {
            dataFiles.push_back(datFilePath);
            editProfile.saveProfile();
            RefreshWindow();
        }
        else // Casc
        {
            if ( datFilePath.ends_with(Sc::DataFile::buildInfoFileName) )
                datFilePath = makeSystemFilePath(getSystemFileDirectory(datFilePath), "Data");

            dataFiles.push_back(datFilePath);
            editProfile.saveProfile();
            RefreshWindow();
        }
    }
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
