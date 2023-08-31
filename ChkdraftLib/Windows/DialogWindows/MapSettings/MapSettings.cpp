#include "MapSettings.h"
#include "../../../Chkdraft.h"

enum_t(Id, u32, {
    WIN_MAPPROPERTIES = ID_FIRST,
    WIN_FORCES,
    WIN_UNITSETTINGS,
    WIN_UPGRADESETTINGS,
    WIN_TECHSETTINGS,
    WIN_STRINGEDITOR,
    WIN_SOUNDEDITOR
});

MapSettingsWindow::MapSettingsWindow() : currTab(Tab::MapProperties)
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
    currTab = Tab::MapProperties;
    ClassDialog::Hide();
    mapPropertiesWindow.DestroyThis();
    forcesWindow.DestroyThis();
    unitSettingsWindow.DestroyThis();
    upgradeSettingsWindow.DestroyThis();
    techSettingsWindow.DestroyThis();
    stringEditorWindow.DestroyThis();
    soundEditorWindow.DestroyThis();
    ClassDialog::DestroyDialog();
    return true;
}

void MapSettingsWindow::ChangeTab(Tab tab)
{
    tabs.SetCurSel((u32)tab);

    ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_MAPPROPERTIES), SW_HIDE);
    ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_FORCES), SW_HIDE);
    ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_UNITSETTINGS), SW_HIDE);
    ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_UPGRADESETTINGS), SW_HIDE);
    ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_TECHSETTINGS), SW_HIDE);
    ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_STRINGEDITOR), SW_HIDE);
    ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_SOUNDEDITOR), SW_HIDE);

    switch ( tab )
    {
        case Tab::MapProperties: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_MAPPROPERTIES), SW_SHOW); break;
        case Tab::Forces: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_FORCES), SW_SHOW); break;
        case Tab::UnitSettings: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_UNITSETTINGS), SW_SHOW); break;
        case Tab::UpgradeSettings: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_UPGRADESETTINGS), SW_SHOW); break;
        case Tab::TechSettings: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_TECHSETTINGS), SW_SHOW); break;
        case Tab::StringEditor: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_STRINGEDITOR), SW_SHOW); break;
        case Tab::SoundEditor: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_SOUNDEDITOR), SW_SHOW); break;
    }

    currTab = tab;
}

void MapSettingsWindow::RefreshWindow()
{
    mapPropertiesWindow.RefreshWindow();
    forcesWindow.RefreshWindow();
    unitSettingsWindow.RefreshWindow();
    upgradeSettingsWindow.RefreshWindow();
    techSettingsWindow.RefreshWindow();
    stringEditorWindow.RefreshWindow();
    soundEditorWindow.RefreshWindow();
}

BOOL MapSettingsWindow::DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    switch ( nmhdr->code )
    {
    case TCN_SELCHANGE:
    {
        Tab selectedTab = (Tab)tabs.GetCurSel();
        switch ( selectedTab )
        {
        case Tab::MapProperties: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_MAPPROPERTIES), SW_SHOW); break;
        case Tab::Forces: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_FORCES), SW_SHOW); break;
        case Tab::UnitSettings: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_UNITSETTINGS), SW_SHOW); break;
        case Tab::UpgradeSettings: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_UPGRADESETTINGS), SW_SHOW); break;
        case Tab::TechSettings: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_TECHSETTINGS), SW_SHOW); break;
        case Tab::StringEditor: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_STRINGEDITOR), SW_SHOW); break;
        case Tab::SoundEditor: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_SOUNDEDITOR), SW_SHOW); break;
        }
    }
    break;

    case TCN_SELCHANGING:
    {
        Tab selectedTab = (Tab)tabs.GetCurSel();
        switch ( selectedTab )
        {
        case Tab::MapProperties: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_MAPPROPERTIES), SW_HIDE); break;
        case Tab::Forces: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_FORCES), SW_HIDE); break;
        case Tab::UnitSettings: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_UNITSETTINGS), SW_HIDE); break;
        case Tab::UpgradeSettings: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_UPGRADESETTINGS), SW_HIDE); break;
        case Tab::TechSettings: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_TECHSETTINGS), SW_HIDE); break;
        case Tab::StringEditor: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_STRINGEDITOR), SW_HIDE); break;
        case Tab::SoundEditor: ShowWindow(GetDlgItem(tabs.getHandle(), Id::WIN_SOUNDEDITOR), SW_HIDE); break;
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
                const std::vector<std::string> tabTitles = {
                    "Map Properties", "Forces", "Unit Settings", "Upgrade Settings",
                    "Tech Settings", "String Editor", "Sound Editor"
                };

                tabs.FindThis(getHandle(), IDC_MAPSETTINGSTABS);
                HICON icon = WinLib::ResourceManager::getIcon(IDI_PROGRAM_ICON, 16, 16);
                tabs.SetSmallIcon(icon);
                tabs.setDefaultFont(false);
                
                TCITEM item = { };
                item.mask = TCIF_TEXT;
                item.iImage = -1;
                
                for ( u32 i=0; i<tabTitles.size(); i++ )
                    tabs.InsertTab(i, tabTitles[i]);
                
                mapPropertiesWindow.CreateThis(tabs.getHandle(), Id::WIN_MAPPROPERTIES);
                forcesWindow.CreateThis(tabs.getHandle(), Id::WIN_FORCES);
                unitSettingsWindow.CreateThis(tabs.getHandle(), Id::WIN_UNITSETTINGS);
                upgradeSettingsWindow.CreateThis(tabs.getHandle(), Id::WIN_UPGRADESETTINGS);
                techSettingsWindow.CreateThis(tabs.getHandle(), Id::WIN_TECHSETTINGS);
                stringEditorWindow.CreateThis(tabs.getHandle(), Id::WIN_STRINGEDITOR);
                soundEditorWindow.CreateThis(tabs.getHandle(), Id::WIN_SOUNDEDITOR);
                WindowsItem::defaultChildFonts();
                RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
            }
            break;

        case WM_MOUSEWHEEL:
            if ( currTab == Tab::StringEditor )
                SendMessage(GetDlgItem(tabs.getHandle(), Id::WIN_STRINGEDITOR), WM_MOUSEWHEEL, wParam, lParam);
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
