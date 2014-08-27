#include "MapSettings.h"

MapSettingsWindow::MapSettingsWindow() : currTab(0)
{

}

bool MapSettingsWindow::CreateThis(HWND hParent)
{
	return ClassWindow::CreateModelessDialog(MAKEINTRESOURCE(IDD_MAPSETTINGS), hParent);
}

void MapSettingsWindow::ChangeTab(u32 tabID)
{
	HWND hWnd = getHandle();
	TabCtrl_SetCurSel(GetDlgItem(hWnd, IDC_MAPSETTINGSTABS), tabID);

	ShowWindow(GetDlgItem(hWnd, ID_MAPPROPERTIES), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, ID_FORCES), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, ID_UNITSETTINGS), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, ID_UPGRADESETTINGS), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, ID_TECHSETTINGS), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, ID_STRINGEDITOR), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, ID_WAVEDITOR), SW_HIDE);

	switch ( tabID )
	{
		case ID_TAB_MAPPROPERTIES: ShowWindow(GetDlgItem(hWnd, ID_MAPPROPERTIES), SW_SHOW); break;
		case ID_TAB_FORCES: ShowWindow(GetDlgItem(hWnd, ID_FORCES), SW_SHOW); break;
		case ID_TAB_UNITSETTINGS: ShowWindow(GetDlgItem(hWnd, ID_UNITSETTINGS), SW_SHOW); break;
		case ID_TAB_UPGRADESETTINGS: ShowWindow(GetDlgItem(hWnd, ID_UPGRADESETTINGS), SW_SHOW); break;
		case ID_TAB_TECHSETTINGS: ShowWindow(GetDlgItem(hWnd, ID_TECHSETTINGS), SW_SHOW); break;
		case ID_TAB_STRINGEDITOR: ShowWindow(GetDlgItem(hWnd, ID_STRINGEDITOR), SW_SHOW); break;
		case ID_TAB_WAVEDITOR: ShowWindow(GetDlgItem(hWnd, ID_WAVEDITOR), SW_SHOW); break;
	}

	currTab = tabID;
}

BOOL MapSettingsWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_INITDIALOG:
			{
				const char* tabs[7] = {
					"Map Properties", "Forces", "Unit Settings", "Upgrade Settings",
					"Tech Settings", "String Editor", "Wav Editor"
				};

				HWND hTabs = GetDlgItem(hWnd, IDC_MAPSETTINGSTABS);
				LPARAM icon = (LPARAM)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PROGRAM_ICON), IMAGE_ICON, 16, 16, 0);
				SendMessage(hTabs, WM_SETFONT, (WPARAM)defaultFont, FALSE);
				SendMessage(hWnd, WM_SETICON, ICON_SMALL, icon);

				TCITEM item = { };
				item.mask = TCIF_TEXT;
				item.iImage = -1;
				
				for ( int i=0; i<7; i++ )
				{
					item.pszText = (LPSTR)tabs[i];
					item.cchTextMax = strlen(tabs[i]);
					TabCtrl_InsertItem(hTabs, i, &item);
				}
				
				mapPropertiesWindow.CreateThis(hWnd);
				forcesWindow.CreateThis(hWnd);
				unitSettingsWindow.CreateThis(hWnd);
				upgradeSettingsWindow.CreateThis(hWnd);
				techSettingsWindow.CreateThis(hWnd);
				stringEditorWindow.CreateThis(hWnd);
				wavEditorWindow.CreateThis(hWnd);
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
			}
			break;

		case WM_MOUSEWHEEL:
			if ( currTab == ID_TAB_STRINGEDITOR )
				SendMessage(GetDlgItem(hWnd, ID_STRINGEDITOR), WM_MOUSEWHEEL, wParam, lParam);
			break;

		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					break;
				
				default:
					return DefWindowProc(hWnd, msg, wParam, lParam);
					break;
			}
			break;

		case WM_NOTIFY:
			{
				NMHDR* notification = (NMHDR*)lParam;
				switch ( notification->code )
				{
					case TCN_SELCHANGE:
						{
							HWND hTabs = GetDlgItem(hWnd, IDC_MAPSETTINGSTABS);
							u32 selectedTab = TabCtrl_GetCurSel(hTabs);
							switch ( selectedTab )
							{
								case ID_TAB_MAPPROPERTIES: ShowWindow(GetDlgItem(hWnd, ID_MAPPROPERTIES), SW_SHOW); break;
								case ID_TAB_FORCES: ShowWindow(GetDlgItem(hWnd, ID_FORCES), SW_SHOW); break;
								case ID_TAB_UNITSETTINGS: ShowWindow(GetDlgItem(hWnd, ID_UNITSETTINGS), SW_SHOW); break;
								case ID_TAB_UPGRADESETTINGS: ShowWindow(GetDlgItem(hWnd, ID_UPGRADESETTINGS), SW_SHOW); break;
								case ID_TAB_TECHSETTINGS: ShowWindow(GetDlgItem(hWnd, ID_TECHSETTINGS), SW_SHOW); break;
								case ID_TAB_STRINGEDITOR: ShowWindow(GetDlgItem(hWnd, ID_STRINGEDITOR), SW_SHOW); break;
								case ID_TAB_WAVEDITOR: ShowWindow(GetDlgItem(hWnd, ID_WAVEDITOR), SW_SHOW); break;
							}
						}
						break;

					case TCN_SELCHANGING:
						{
							HWND hTabs = GetDlgItem(hWnd, IDC_MAPSETTINGSTABS);
							u32 selectedTab = TabCtrl_GetCurSel(hTabs);
							switch ( selectedTab )
							{
								case ID_TAB_MAPPROPERTIES: ShowWindow(GetDlgItem(hWnd, ID_MAPPROPERTIES), SW_HIDE); break;
								case ID_TAB_FORCES: ShowWindow(GetDlgItem(hWnd, ID_FORCES), SW_HIDE); break;
								case ID_TAB_UNITSETTINGS: ShowWindow(GetDlgItem(hWnd, ID_UNITSETTINGS), SW_HIDE); break;
								case ID_TAB_UPGRADESETTINGS: ShowWindow(GetDlgItem(hWnd, ID_UPGRADESETTINGS), SW_HIDE); break;
								case ID_TAB_TECHSETTINGS: ShowWindow(GetDlgItem(hWnd, ID_TECHSETTINGS), SW_HIDE); break;
								case ID_TAB_STRINGEDITOR: ShowWindow(GetDlgItem(hWnd, ID_STRINGEDITOR), SW_HIDE); break;
								case ID_TAB_WAVEDITOR: ShowWindow(GetDlgItem(hWnd, ID_WAVEDITOR), SW_HIDE); break;
							}
						}
						break;
				}
			}
			break;

		case REFRESH_WINDOW:
			SendMessage(GetDlgItem(hWnd, ID_MAPPROPERTIES), REFRESH_WINDOW, NULL, NULL);
			SendMessage(GetDlgItem(hWnd, ID_FORCES), REFRESH_WINDOW, NULL, NULL);
			SendMessage(GetDlgItem(hWnd, ID_UNITSETTINGS), REFRESH_WINDOW, NULL, NULL);
			SendMessage(GetDlgItem(hWnd, ID_UPGRADESETTINGS), REFRESH_WINDOW, NULL, NULL);
			SendMessage(GetDlgItem(hWnd, ID_TECHSETTINGS), REFRESH_WINDOW, NULL, NULL);
			SendMessage(GetDlgItem(hWnd, ID_STRINGEDITOR), REFRESH_WINDOW, NULL, NULL);
			SendMessage(GetDlgItem(hWnd, ID_WAVEDITOR), REFRESH_WINDOW, NULL, NULL);
			break;
	}

	return FALSE;
}
