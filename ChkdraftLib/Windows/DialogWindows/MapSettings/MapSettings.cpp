#include "MapSettings.h"
#include "../../../Chkdraft.h"

enum ID {
    WIN_MAPPROPERTIES = ID_FIRST,
    WIN_FORCES,
    WIN_UNITSETTINGS,
    WIN_UPGRADESETTINGS,
    WIN_TECHSETTINGS,
    WIN_STRINGEDITOR,
    WIN_WAVEDITOR
};

MapSettingsWindow::MapSettingsWindow() : currTab(0)
{

}

MapSettingsWindow::~MapSettingsWindow()
{

}

bool MapSettingsWindow::CreateThis(HWND hParent)
{
    return ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_MAPSETTINGS), hParent);
}

bool MapSettingsWindow::DestroyThis()
{
    currTab = 0;
    ClassDialog::DestroyDialog();
    return true;
}

void MapSettingsWindow::ChangeTab(u32 tabID)
{
    tabs.SetCurSel(tabID);

    ShowWindow(GetDlgItem(tabs.getHandle(), WIN_MAPPROPERTIES), SW_HIDE);
    ShowWindow(GetDlgItem(tabs.getHandle(), WIN_FORCES), SW_HIDE);
    ShowWindow(GetDlgItem(tabs.getHandle(), WIN_UNITSETTINGS), SW_HIDE);
    ShowWindow(GetDlgItem(tabs.getHandle(), WIN_UPGRADESETTINGS), SW_HIDE);
    ShowWindow(GetDlgItem(tabs.getHandle(), WIN_TECHSETTINGS), SW_HIDE);
    ShowWindow(GetDlgItem(tabs.getHandle(), WIN_STRINGEDITOR), SW_HIDE);
    ShowWindow(GetDlgItem(tabs.getHandle(), WIN_WAVEDITOR), SW_HIDE);

    switch ( tabID )
    {
        case MapSettings::TAB_MAPPROPERTIES: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_MAPPROPERTIES), SW_SHOW); break;
        case MapSettings::TAB_FORCES: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_FORCES), SW_SHOW); break;
        case MapSettings::TAB_UNITSETTINGS: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_UNITSETTINGS), SW_SHOW); break;
        case MapSettings::TAB_UPGRADESETTINGS: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_UPGRADESETTINGS), SW_SHOW); break;
        case MapSettings::TAB_TECHSETTINGS: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_TECHSETTINGS), SW_SHOW); break;
        case MapSettings::TAB_STRINGEDITOR: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_STRINGEDITOR), SW_SHOW); break;
        case MapSettings::TAB_WAVEDITOR: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_WAVEDITOR), SW_SHOW); break;
    }

    currTab = tabID;
}

void MapSettingsWindow::RefreshWindow()
{
    mapPropertiesWindow.RefreshWindow();
    forcesWindow.RefreshWindow();
    unitSettingsWindow.RefreshWindow();
    upgradeSettingsWindow.RefreshWindow();
    techSettingsWindow.RefreshWindow();
    stringEditorWindow.RefreshWindow();
    wavEditorWindow.RefreshWindow();
}

BOOL MapSettingsWindow::DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    switch ( nmhdr->code )
    {
    case TCN_SELCHANGE:
    {
        u32 selectedTab = tabs.GetCurSel();
        switch ( selectedTab )
        {
        case TAB_MAPPROPERTIES: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_MAPPROPERTIES), SW_SHOW); break;
        case TAB_FORCES: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_FORCES), SW_SHOW); break;
        case TAB_UNITSETTINGS: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_UNITSETTINGS), SW_SHOW); break;
        case TAB_UPGRADESETTINGS: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_UPGRADESETTINGS), SW_SHOW); break;
        case TAB_TECHSETTINGS: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_TECHSETTINGS), SW_SHOW); break;
        case TAB_STRINGEDITOR: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_STRINGEDITOR), SW_SHOW); break;
        case TAB_WAVEDITOR: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_WAVEDITOR), SW_SHOW); break;
        }
    }
    break;

    case TCN_SELCHANGING:
    {
        u32 selectedTab = tabs.GetCurSel();
        switch ( selectedTab )
        {
        case TAB_MAPPROPERTIES: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_MAPPROPERTIES), SW_HIDE); break;
        case TAB_FORCES: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_FORCES), SW_HIDE); break;
        case TAB_UNITSETTINGS: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_UNITSETTINGS), SW_HIDE); break;
        case TAB_UPGRADESETTINGS: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_UPGRADESETTINGS), SW_HIDE); break;
        case TAB_TECHSETTINGS: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_TECHSETTINGS), SW_HIDE); break;
        case TAB_STRINGEDITOR: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_STRINGEDITOR), SW_HIDE); break;
        case TAB_WAVEDITOR: ShowWindow(GetDlgItem(tabs.getHandle(), WIN_WAVEDITOR), SW_HIDE); break;
        }
    }
    break;
    default:
        return FALSE;
        break;
    }
    return TRUE;
}

BOOL MapSettingsWindow::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

BOOL MapSettingsWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_ACTIVATE:
            if ( LOWORD(wParam) != WA_INACTIVE )
                chkd.SetCurrDialog(hWnd);
            break;

        case WM_INITDIALOG:
            {
                const char* tabTitles[7] = {
                    "Map Properties", "Forces", "Unit Settings", "Upgrade Settings",
                    "Tech Settings", "String Editor", "Wav Editor"
                };

                tabs.FindThis(getHandle(), IDC_MAPSETTINGSTABS);
                HANDLE icon = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PROGRAM_ICON), IMAGE_ICON, 16, 16, 0);
                tabs.SetSmallIcon(icon);
                tabs.SetFont(defaultFont, false);
                


                TCITEM item = { };
                item.mask = TCIF_TEXT;
                item.iImage = -1;
                
                for ( int i=0; i<sizeof(tabs)/sizeof(const char*); i++ )
                    tabs.InsertTab(i, tabTitles[i]);
                
                mapPropertiesWindow.CreateThis(tabs.getHandle(), WIN_MAPPROPERTIES);
                forcesWindow.CreateThis(tabs.getHandle(), WIN_FORCES);
                unitSettingsWindow.CreateThis(tabs.getHandle(), WIN_UNITSETTINGS);
                upgradeSettingsWindow.CreateThis(tabs.getHandle(), WIN_UPGRADESETTINGS);
                techSettingsWindow.CreateThis(tabs.getHandle(), WIN_TECHSETTINGS);
                stringEditorWindow.CreateThis(tabs.getHandle(), WIN_STRINGEDITOR);
                wavEditorWindow.CreateThis(tabs.getHandle(), WIN_WAVEDITOR);
                ReplaceChildFonts(defaultFont);
                RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
            }
            break;

        case WM_MOUSEWHEEL:
            if ( currTab == MapSettings::TAB_STRINGEDITOR )
                SendMessage(GetDlgItem(tabs.getHandle(), WIN_STRINGEDITOR), WM_MOUSEWHEEL, wParam, lParam);
            else
                return FALSE;
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
