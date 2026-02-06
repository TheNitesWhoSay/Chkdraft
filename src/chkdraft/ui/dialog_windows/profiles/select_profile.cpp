#include "select_profile.h"
#include "chkdraft/chkdraft.h"
#include <mapping_core/mapping_core.h>

enum_t(Id, u32, {
    ListProfiles = IDC_LIST_PROFILES
});

SelectProfile::~SelectProfile()
{

}

void SelectProfile::CreateThis(HWND hParent)
{
    ClassDialog::CreateDialogBox(MAKEINTRESOURCE(IDD_DIALOG_SELPROFILE), hParent);
}

void SelectProfile::RefreshWindow()
{
    listBoxProfiles.ClearSel();
    listBoxProfiles.ClearItems();
    auto curr = chkd.profiles.getCurrProfile();
    int selIndex = -1;
    for ( std::size_t i=0; i<chkd.profiles.profiles.size(); ++i )
    {
        if ( curr != nullptr && curr == chkd.profiles.profiles[i].get() )
            selIndex = int(i);

        listBoxProfiles.AddString(chkd.profiles[i].profileName);
    }
    if ( selIndex != -1 )
        listBoxProfiles.SetCurSel(selIndex);
}

BOOL SelectProfile::DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    return FALSE;
}

BOOL SelectProfile::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( HIWORD(wParam) )
    {
    case LBN_SELCHANGE:
        {
            std::string selectedProfile = "";
            if ( listBoxProfiles.GetCurSelString(selectedProfile) )
            {
                // Could maybe update visuals in response to selecting a profile, there's nothing strictly needed here though
            }
        }
        break;
    case LBN_DBLCLK:
        if ( Id::ListProfiles == LOWORD(wParam) )
        {
            int selectedProfile = -1;
            if ( listBoxProfiles.GetCurSel(selectedProfile) )
                LoadProfile(selectedProfile);
        }
        break;
    default:
        switch ( LOWORD(wParam) )
        {
            case IDCANCEL:
                ExitPressed();
                break;
            case IDOK:
                {
                    int selectedProfile = -1;
                    if ( listBoxProfiles.GetCurSel(selectedProfile) )
                        LoadProfile(selectedProfile);
                }
                break;
            case IDC_BUTTON_FINDPROFILE:
                FindProfile();
                break;
            case IDC_BUTTON_EDITPROFILES:
                EditProfiles();
                break;
        }
        break;
    }
    return TRUE;
}

BOOL SelectProfile::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_INITDIALOG:
            {
                listBoxProfiles.FindThis(hWnd, IDC_LIST_PROFILES);
                RefreshWindow();
            }
            break;
        case WM_ACTIVATE:
            if ( wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE )
                RefreshWindow();
            break;
    }
    return FALSE;
}

void SelectProfile::LoadProfile(int profileIndex)
{
    chkd.profiles.setCurrProfile(profileIndex);
    if ( chkd.OnProfileLoad() )
    {
        chkd.UpdateProfilesMenu();
        EndDialog(getHandle(), IDOK);
    }
}

void SelectProfile::FindProfile()
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

                if ( chkd.editProfilesWindow && chkd.editProfilesWindow->getHandle() != NULL )
                    chkd.editProfilesWindow->RefreshWindow();

                chkd.UpdateProfilesMenu();
            }
        }
    }
}

void SelectProfile::EditProfiles()
{
    chkd.OpenEditProfilesDialog();
}

void SelectProfile::ExitPressed()
{
    EndDialog(getHandle(), IDCANCEL);
    PostQuitMessage(0);
}
