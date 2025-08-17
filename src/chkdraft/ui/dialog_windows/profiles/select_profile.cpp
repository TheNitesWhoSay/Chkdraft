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
            std::string selectedProfile = "";
            if ( listBoxProfiles.GetCurSelString(selectedProfile) )
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
                    std::string selectedProfile = "";
                    if ( listBoxProfiles.GetCurSelString(selectedProfile) )
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
                listBoxProfiles.AddStrings(std::vector<std::string>{"first", "second", "third"});
            }
            break;
    }
    return FALSE;
}

void SelectProfile::LoadProfile(const std::string & profileName)
{
    // TODO: Figure out what profile is selected, try to load it, if successful close the dialog, else display the error
    logger.info() << "Loading profile: " << profileName << '\n';
}

void SelectProfile::FindProfile()
{
    // TODO: Open a browser for the user to select a profile .json, if successful, make it the currently selected profile
    logger.info() << "FindProfile...\n";
}

void SelectProfile::EditProfiles()
{
    // TODO: Open the edit profiles dialog
    logger.info() << "EditProfiles...\n";
}

void SelectProfile::ExitPressed()
{
    // TODO: Depending on the context, this may mean exit the program not just the dialog
    EndDialog(getHandle(), IDCANCEL);
}
