#include "edit_profiles.h"
#include "chkdraft.h"

enum_t(Id, u32, {
    WIN_PROFILEPATHS = ID_FIRST,
    WIN_CUSTOMDATFILES,
    WIN_EDITORSETTINGS
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
    profilePathsWindow.DestroyThis();
    customDatFilesWindow.DestroyThis();
    editorSettingsWindow.DestroyThis();
    ClassDialog::DestroyDialog();
    this->currTab = Tab::ProfilePaths;
}

void EditProfilesWindow::ChangeTab(Tab tab)
{
    tabs.SetCurSel((u32)tab);

    ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_PROFILEPATHS), SW_HIDE);
    ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_CUSTOMDATFILES), SW_HIDE);
    ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_EDITORSETTINGS), SW_HIDE);

    switch ( tab )
    {
        case Tab::ProfilePaths: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_PROFILEPATHS), SW_SHOW); break;
        case Tab::CustomDatFiles: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_CUSTOMDATFILES), SW_SHOW); break;
        case Tab::EditorSettings: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_EDITORSETTINGS), SW_SHOW); break;
    }

    currTab = tab;
}

void EditProfilesWindow::RefreshWindow()
{
    profilePathsWindow.RefreshWindow();
    customDatFilesWindow.RefreshWindow();
    editorSettingsWindow.RefreshWindow();
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
        case Tab::ProfilePaths: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_PROFILEPATHS), SW_SHOW); break;
        case Tab::CustomDatFiles: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_CUSTOMDATFILES), SW_SHOW); break;
        case Tab::EditorSettings: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_EDITORSETTINGS), SW_SHOW); break;
        }
    }
    break;

    case TCN_SELCHANGING:
    {
        Tab selectedTab = (Tab)tabs.GetCurSel();
        switch ( selectedTab )
        {
        case Tab::ProfilePaths: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_PROFILEPATHS), SW_HIDE); break;
        case Tab::CustomDatFiles: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_CUSTOMDATFILES), SW_HIDE); break;
        case Tab::EditorSettings: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_EDITORSETTINGS), SW_HIDE); break;
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
                const std::vector<std::string> tabTitles = {
                    "Profile Paths", "Custom Dat", "Editor Settings"
                };

                tabs.FindThis(getHandle(), IDC_PROFILE_TABS);
                HICON icon = WinLib::ResourceManager::getIcon(IDI_PROGRAM_ICON, 16, 16);
                tabs.SetSmallIcon(icon);
                tabs.setDefaultFont(false);
                
                TCITEM item = { };
                item.mask = TCIF_TEXT;
                item.iImage = -1;
                
                for ( u32 i=0; i<tabTitles.size(); i++ )
                    tabs.InsertTab(i, tabTitles[i]);
                
                profilePathsWindow.CreateThis(tabs.getHandle(), Id::WIN_PROFILEPATHS);
                customDatFilesWindow.CreateThis(tabs.getHandle(), Id::WIN_CUSTOMDATFILES);
                editorSettingsWindow.CreateThis(tabs.getHandle(), Id::WIN_EDITORSETTINGS);
                WindowsItem::defaultChildFonts();
                ChangeTab(Tab::ProfilePaths);
                RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
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
