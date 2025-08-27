#include "edit_profiles.h"
#include "chkdraft.h"

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
    switch ( LOWORD(wParam) )
    {
    case IDCANCEL:
        EndDialog(hWnd, IDCANCEL);
        break;
    default:
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
