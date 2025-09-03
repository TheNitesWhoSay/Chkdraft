#include "edit_profiles.h"
#include "chkdraft.h"
#include "chkdraft/ui/chkd_controls/input_dialog.h"
#include <filesystem>

enum_t(Id, u32, {
    ProfileTabs = IDC_PROFILE_TABS,
    ProfilePaths = ID_FIRST,
    CustomDatFiles,
    EditorSettings,
    GroupProfiles,
    ListProfiles,
    ButtonAddProfile,
    ButtonCloneProfile,
    ButtonRenameProfile,
    ButtonFindProfile,
    ButtonDeleteProfile,

    CheckDefaultProfile,
    CheckAutoLoadOnStart
});

bool EditProfilesWindow::CreateThis(HWND hParent)
{
    return ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_DIALOG_EDITPROFILES), hParent);
}

void EditProfilesWindow::DestroyThis()
{
    currTab = Tab::ProfilePaths;
    chkd.FocusThis();
    ClassDialog::Hide();
    profileGeneralWindow.DestroyThis();
    customDatFilesWindow.DestroyThis();
    editorSettingsWindow.DestroyThis();
    ClassDialog::DestroyDialog();
    this->currTab = Tab::ProfilePaths;
}

void EditProfilesWindow::ChangeTab(Tab tab)
{
    tabs.SetCurSel((u32)tab);

    ShowWindow(GetDlgItem(tabs.getHandle(), Id::ProfilePaths), SW_HIDE);
    ShowWindow(GetDlgItem(tabs.getHandle(), Id::CustomDatFiles), SW_HIDE);
    ShowWindow(GetDlgItem(tabs.getHandle(), Id::EditorSettings), SW_HIDE);

    switch ( tab )
    {
        case Tab::ProfilePaths: ShowWindow(GetDlgItem(tabs.getHandle(), Id::ProfilePaths), SW_SHOW); break;
        case Tab::CustomDatFiles: ShowWindow(GetDlgItem(tabs.getHandle(), Id::CustomDatFiles), SW_SHOW); break;
        case Tab::EditorSettings: ShowWindow(GetDlgItem(tabs.getHandle(), Id::EditorSettings), SW_SHOW); break;
    }

    currTab = tab;
}

void EditProfilesWindow::RefreshWindow()
{
    profileGeneralWindow.RefreshWindow();
    customDatFilesWindow.RefreshWindow();
    editorSettingsWindow.RefreshWindow();

    listProfiles.ClearSel();
    listProfiles.ClearItems();
    auto & editProfile = getEditProfile();
    std::vector<std::string> profileNames {};
    for ( auto & profile : chkd.profiles.profiles )
        profileNames.push_back(profile->profileName);

    int selIndex = -1;
    if ( !editProfileName.empty() )
    {
        for ( std::size_t i=0; i<std::size(profileNames); ++i )
        {
            if ( editProfileName.compare(profileNames[i]) == 0 )
            {
                selIndex = static_cast<int>(i);
                break;
            }
        }
    }
    listProfiles.AddStrings(profileNames);
    if ( selIndex == -1 )
        editProfileName.clear();
    else
        listProfiles.SetCurSel(selIndex);

    checkIsDefaultProfile.SetCheck(editProfile.isDefaultProfile);
    checkAutoLoadOnStart.SetCheck(editProfile.autoLoadOnStart);
}

void EditProfilesWindow::addProfileClicked()
{
    if ( auto newName = InputDialog::getInput(getHandle(), "Add Profile", "New Profile Name:", "") )
    {
        if ( chkd.profiles.nameUsed(*newName) )
            logger.error() << "A profile named " << (*newName) << " already exists!" << std::endl;
        else if ( auto settingsPath = GetSettingsPath() )
        {
            auto & newProfile = chkd.profiles.profiles.emplace_back(std::make_unique<ChkdProfile>());
            newProfile->profileName = *newName;
            this->editProfileName = newProfile->profileName;
            newProfile->profilePath = makeSystemFilePath(*settingsPath, newProfile->profileName + ".profile.json");
            newProfile->saveProfile();
            chkd.UpdateProfilesMenu();
            RefreshWindow();
        }
    }
}

void EditProfilesWindow::cloneProfileClicked()
{
    const auto & editProfile = getEditProfile();
    if ( auto newName = InputDialog::getInput(getHandle(), "Clone Profile", "New Profile Name:", "") )
    {
        if ( chkd.profiles.nameUsed(*newName) )
            logger.error() << "A profile named " << (*newName) << " already exists!" << std::endl;
        else
        {
            auto & newProfile = chkd.profiles.profiles.emplace_back(std::make_unique<ChkdProfile>(editProfile));
            newProfile->profileName = *newName;
            this->editProfileName = newProfile->profileName;

            std::string directory = getSystemFileDirectory(newProfile->profilePath);
            newProfile->profilePath = makeExtSystemFilePath(makeSystemFilePath(directory, newProfile->profileName), ".profile.json");
            for ( int i=1; std::filesystem::exists(newProfile->profilePath); ++i )
                newProfile->profilePath = makeExtSystemFilePath(makeSystemFilePath(directory, newProfile->profileName + std::to_string(i)), ".profile.json");

            newProfile->saveProfile();
            chkd.UpdateProfilesMenu();
            RefreshWindow();
        }
    }
}

void EditProfilesWindow::renameProfileClicked()
{
    auto & editProfile = getEditProfile();
    if ( auto newName = InputDialog::getInput(getHandle(), "Rename Profile", "Profile Name:", editProfile.profileName) )
    {
        if ( newName != editProfile.profileName )
        {
            if ( chkd.profiles.nameUsed(*newName) )
                logger.error() << "A profile named " << (*newName) << " already exists!" << std::endl;
            else
            {
                editProfile.profileName = *newName;
                this->editProfileName = editProfile.profileName;

                std::string prevPath = editProfile.profilePath;
                std::string directory = getSystemFileDirectory(prevPath);
                editProfile.profilePath = makeExtSystemFilePath(makeSystemFilePath(directory, editProfile.profileName), ".profile.json");
                for ( int i=1; std::filesystem::exists(editProfile.profilePath); ++i )
                    editProfile.profilePath = makeExtSystemFilePath(makeSystemFilePath(directory, editProfile.profileName + std::to_string(i)), ".profile.json");

                std::filesystem::rename(prevPath, editProfile.profilePath);
                editProfile.saveProfile();
                RefreshWindow();
                chkd.ProfileNameUpdated(editProfile);
            }
        }
    }
}

void EditProfilesWindow::findProfileClicked()
{
    std::string profilePath {};
    u32 filterIndex = 0;
    if ( auto settingsPath = GetSettingsPath() )
    {
        if ( browseForFile(profilePath, filterIndex, {{"*.profile.json", "Chkd Profile"}}, *settingsPath, "Find Profile", true, false) &&
            profilePath.ends_with(".profile.json") )
        {
            if ( chkd.profiles.loadProfile(profilePath) != nullptr )
            {
                for ( auto & profile : chkd.profiles.profiles )
                {
                    profile->additionalProfileDirectories.push_back(getSystemFileDirectory(profilePath));
                    profile->saveProfile();
                }
                RefreshWindow();
                chkd.UpdateProfilesMenu();
            }
        }
    }
}

void EditProfilesWindow::deleteProfileClicked()
{
    if ( chkd.profiles.profiles.size() == 1 )
    {
        logger.error("Cannot delete the last profile");
        return;
    }

    auto & editProfile = getEditProfile();
    std::size_t editProfileIndex = std::numeric_limits<std::size_t>::max();
    for ( std::size_t i=0; i<chkd.profiles.profiles.size(); ++i )
    {
        if ( chkd.profiles[i].profilePath == editProfile.profilePath )
        {
            editProfileIndex = i;
            break;
        }
    }
    chkd.RemoveProfileFromMenu(editProfile);

    bool wasCurrentProfile = editProfile.profilePath == chkd.profiles().profilePath;
    if ( wasCurrentProfile )
    {
        for ( std::size_t i=0; i<chkd.profiles.profiles.size(); ++i )
        {
            if ( i != editProfileIndex )
            {
                chkd.profiles.setCurrProfile(i);
                break;
            }
        }
    }

    std::string profilePath = editProfile.profilePath;
    chkd.profiles.profiles.erase(std::next(chkd.profiles.profiles.begin(), static_cast<std::ptrdiff_t>(editProfileIndex)));
    std::filesystem::remove(profilePath);
    chkd.UpdateProfilesMenu();
    if ( wasCurrentProfile )
        chkd.OnProfileLoad();
    else
        RefreshWindow();
}

void EditProfilesWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
        case Id::ButtonAddProfile: addProfileClicked(); break;
        case Id::ButtonCloneProfile: cloneProfileClicked(); break;
        case Id::ButtonRenameProfile: renameProfileClicked(); break;
        case Id::ButtonFindProfile: findProfileClicked(); break;
        case Id::ButtonDeleteProfile: deleteProfileClicked(); break;
        case Id::CheckDefaultProfile:
        {
            auto & editProfile = getEditProfile();
            editProfile.isDefaultProfile = checkIsDefaultProfile.isChecked();
            editProfile.saveProfile();
        }
        break;
        case Id::CheckAutoLoadOnStart:
        {
            auto & editProfile = getEditProfile();
            editProfile.autoLoadOnStart = checkAutoLoadOnStart.isChecked();
            editProfile.saveProfile();
        }
        break;
    }
}

BOOL EditProfilesWindow::DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    switch ( nmhdr->code )
    {
    case TCN_SELCHANGE:
    {
        Tab selectedTab = (Tab)tabs.GetCurSel();
        switch ( selectedTab )
        {
        case Tab::ProfilePaths: ShowWindow(GetDlgItem(tabs.getHandle(), Id::ProfilePaths), SW_SHOW); break;
        case Tab::CustomDatFiles: ShowWindow(GetDlgItem(tabs.getHandle(), Id::CustomDatFiles), SW_SHOW); break;
        case Tab::EditorSettings: ShowWindow(GetDlgItem(tabs.getHandle(), Id::EditorSettings), SW_SHOW); break;
        }
    }
    break;

    case TCN_SELCHANGING:
    {
        Tab selectedTab = (Tab)tabs.GetCurSel();
        switch ( selectedTab )
        {
        case Tab::ProfilePaths: ShowWindow(GetDlgItem(tabs.getHandle(), Id::ProfilePaths), SW_HIDE); break;
        case Tab::CustomDatFiles: ShowWindow(GetDlgItem(tabs.getHandle(), Id::CustomDatFiles), SW_HIDE); break;
        case Tab::EditorSettings: ShowWindow(GetDlgItem(tabs.getHandle(), Id::EditorSettings), SW_HIDE); break;
        }
    }
    break;
    default:
        return FALSE;
        break;
    }
    return TRUE;
}

BOOL EditProfilesWindow::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( HIWORD(wParam) )
    {
        case LBN_SELCHANGE:
            if ( LOWORD(wParam) == Id::ListProfiles )
            {
                std::string newSelProfile {};
                if ( listProfiles.GetCurSelString(newSelProfile) )
                {
                    this->editProfileName = newSelProfile;
                    RefreshWindow();
                }
            }
            break;
        default:
            switch ( LOWORD(wParam) )
            {
                case IDCANCEL:
                    EndDialog(hWnd, IDCANCEL);
                    break;
            }
            return FALSE;
            break;
    }
    return TRUE;
}

BOOL EditProfilesWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_ACTIVATE:
            if ( LOWORD(wParam) != WA_INACTIVE )
                chkd.SetCurrDialog(hWnd);
            break;

        case WM_INITDIALOG:
            {
                groupProfiles.CreateThis(hWnd, 4, 0, 275, 98, "Profiles", Id::GroupProfiles);
                const std::vector<std::string> tabTitles = {
                    "General", "Data Files" //, "Map Auto-Load", "View", "Editor"
                };
                listProfiles.CreateThis(hWnd, 8, 14, 267, 84, false, false, true, false, true, Id::ListProfiles);
                buttonRenameProfile.CreateThis(hWnd, 281, 6, 100, 20, "Rename Profile", Id::ButtonRenameProfile, false, false);
                buttonCloneProfile.CreateThis(hWnd, 281, buttonRenameProfile.Bottom()+2, 100, 20, "Clone Profile", Id::ButtonCloneProfile, false, false);
                buttonDeleteProfile.CreateThis(hWnd, 281, buttonCloneProfile.Bottom()+2, 100, 20, "Delete Profile", Id::ButtonDeleteProfile, false, false);
                buttonAddProfile.CreateThis(hWnd, buttonRenameProfile.Right()+2, 6, 100, 20, "Add Profile", Id::ButtonAddProfile, false, false);
                buttonFindProfile.CreateThis(hWnd, buttonRenameProfile.Right()+2, buttonAddProfile.Bottom()+2, 100, 20, "Find Profile...", Id::ButtonFindProfile, false, false);
                
                checkIsDefaultProfile.CreateThis(hWnd, buttonFindProfile.Left(), buttonFindProfile.Bottom()+2, 100, 20, true, "Default Profile", Id::CheckDefaultProfile);
                checkAutoLoadOnStart.CreateThis(hWnd, checkIsDefaultProfile.Left(), checkIsDefaultProfile.Bottom()+2, 125, 20, true, "Auto-Load On Start", Id::CheckAutoLoadOnStart);

                tabs.FindThis(getHandle(), IDC_PROFILE_TABS);
                HICON icon = WinLib::ResourceManager::getIcon(IDI_PROGRAM_ICON, 16, 16);
                tabs.SetSmallIcon(icon);
                tabs.setDefaultFont(false);
                
                TCITEM item = { };
                item.mask = TCIF_TEXT;
                item.iImage = -1;
                
                for ( u32 i=0; i<tabTitles.size(); i++ )
                    tabs.InsertTab(i, tabTitles[i]);
                
                profileGeneralWindow.CreateThis(tabs.getHandle(), Id::ProfilePaths);
                customDatFilesWindow.CreateThis(tabs.getHandle(), Id::CustomDatFiles);
                editorSettingsWindow.CreateThis(tabs.getHandle(), Id::EditorSettings);

                WindowsItem::defaultChildFonts();
                ChangeTab(Tab::ProfilePaths);
                RefreshWindow();
            }
            break;

        case WM_CLOSE:
            DestroyThis();
            break;

        default:
            return FALSE;
            break;
    }

    return TRUE;
}

ChkdProfile & EditProfilesWindow::getEditProfile()
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
