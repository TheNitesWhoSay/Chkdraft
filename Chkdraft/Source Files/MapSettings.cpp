#include "MapSettings.h"
#include "Chkdraft.h"

MapSettingsWindow::MapSettingsWindow() : currTab(0), hTabs(NULL)
{

}

bool MapSettingsWindow::CreateThis(HWND hParent)
{
	return ClassWindow::CreateModelessDialog(MAKEINTRESOURCE(IDD_MAPSETTINGS), hParent);
}

bool MapSettingsWindow::DestroyThis()
{
	currTab = 0;
	hTabs = NULL;
	ClassWindow::DestroyDialog();
	return true;
}

void MapSettingsWindow::ChangeTab(u32 tabID)
{
	TabCtrl_SetCurSel(hTabs, tabID);	

	ShowWindow(GetDlgItem(hTabs, ID_MAPPROPERTIES), SW_HIDE);
	ShowWindow(GetDlgItem(hTabs, ID_FORCES), SW_HIDE);
	ShowWindow(GetDlgItem(hTabs, ID_UNITSETTINGS), SW_HIDE);
	ShowWindow(GetDlgItem(hTabs, ID_UPGRADESETTINGS), SW_HIDE);
	ShowWindow(GetDlgItem(hTabs, ID_TECHSETTINGS), SW_HIDE);
	ShowWindow(GetDlgItem(hTabs, ID_STRINGEDITOR), SW_HIDE);
	ShowWindow(GetDlgItem(hTabs, ID_WAVEDITOR), SW_HIDE);

	switch ( tabID )
	{
		case ID_TAB_MAPPROPERTIES: ShowWindow(GetDlgItem(hTabs, ID_MAPPROPERTIES), SW_SHOW); break;
		case ID_TAB_FORCES: ShowWindow(GetDlgItem(hTabs, ID_FORCES), SW_SHOW); break;
		case ID_TAB_UNITSETTINGS: ShowWindow(GetDlgItem(hTabs, ID_UNITSETTINGS), SW_SHOW); break;
		case ID_TAB_UPGRADESETTINGS: ShowWindow(GetDlgItem(hTabs, ID_UPGRADESETTINGS), SW_SHOW); break;
		case ID_TAB_TECHSETTINGS: ShowWindow(GetDlgItem(hTabs, ID_TECHSETTINGS), SW_SHOW); break;
		case ID_TAB_STRINGEDITOR: ShowWindow(GetDlgItem(hTabs, ID_STRINGEDITOR), SW_SHOW); break;
		case ID_TAB_WAVEDITOR: ShowWindow(GetDlgItem(hTabs, ID_WAVEDITOR), SW_SHOW); break;
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
				const char* tabs[7] = {
					"Map Properties", "Forces", "Unit Settings", "Upgrade Settings",
					"Tech Settings", "String Editor", "Wav Editor"
				};

				hTabs = GetDlgItem(hWnd, IDC_MAPSETTINGSTABS);
				LPARAM icon = (LPARAM)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PROGRAM_ICON), IMAGE_ICON, 16, 16, 0);
				SendMessage(hTabs, WM_SETFONT, (WPARAM)defaultFont, FALSE);
				SendMessage(hWnd, WM_SETICON, ICON_SMALL, icon);

				TCITEM item = { };
				item.mask = TCIF_TEXT;
				item.iImage = -1;
				
				for ( int i=0; i<sizeof(tabs)/sizeof(const char*); i++ )
				{
					item.pszText = (LPSTR)tabs[i];
					item.cchTextMax = strlen(tabs[i]);
					TabCtrl_InsertItem(hTabs, i, &item);
				}
				
				mapPropertiesWindow.CreateThis(hTabs);
				forcesWindow.CreateThis(hTabs);
				unitSettingsWindow.CreateThis(hTabs);
				upgradeSettingsWindow.CreateThis(hTabs);
				techSettingsWindow.CreateThis(hTabs);
				stringEditorWindow.CreateThis(hTabs);
				wavEditorWindow.CreateThis(hTabs);
				ReplaceChildFonts(defaultFont);
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
			}
			break;

		case WM_MOUSEWHEEL:
			if ( currTab == ID_TAB_STRINGEDITOR )
				SendMessage(GetDlgItem(hTabs, ID_STRINGEDITOR), WM_MOUSEWHEEL, wParam, lParam);
			else
				return FALSE;
			break;

		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					break;
				default:
					return FALSE;
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
							u32 selectedTab = TabCtrl_GetCurSel(hTabs);
							switch ( selectedTab )
							{
								case ID_TAB_MAPPROPERTIES: ShowWindow(GetDlgItem(hTabs, ID_MAPPROPERTIES), SW_SHOW); break;
								case ID_TAB_FORCES: ShowWindow(GetDlgItem(hTabs, ID_FORCES), SW_SHOW); break;
								case ID_TAB_UNITSETTINGS: ShowWindow(GetDlgItem(hTabs, ID_UNITSETTINGS), SW_SHOW); break;
								case ID_TAB_UPGRADESETTINGS: ShowWindow(GetDlgItem(hTabs, ID_UPGRADESETTINGS), SW_SHOW); break;
								case ID_TAB_TECHSETTINGS: ShowWindow(GetDlgItem(hTabs, ID_TECHSETTINGS), SW_SHOW); break;
								case ID_TAB_STRINGEDITOR: ShowWindow(GetDlgItem(hTabs, ID_STRINGEDITOR), SW_SHOW); break;
								case ID_TAB_WAVEDITOR: ShowWindow(GetDlgItem(hTabs, ID_WAVEDITOR), SW_SHOW); break;
							}
						}
						break;

					case TCN_SELCHANGING:
						{
							u32 selectedTab = TabCtrl_GetCurSel(hTabs);
							switch ( selectedTab )
							{
								case ID_TAB_MAPPROPERTIES: ShowWindow(GetDlgItem(hTabs, ID_MAPPROPERTIES), SW_HIDE); break;
								case ID_TAB_FORCES: ShowWindow(GetDlgItem(hTabs, ID_FORCES), SW_HIDE); break;
								case ID_TAB_UNITSETTINGS: ShowWindow(GetDlgItem(hTabs, ID_UNITSETTINGS), SW_HIDE); break;
								case ID_TAB_UPGRADESETTINGS: ShowWindow(GetDlgItem(hTabs, ID_UPGRADESETTINGS), SW_HIDE); break;
								case ID_TAB_TECHSETTINGS: ShowWindow(GetDlgItem(hTabs, ID_TECHSETTINGS), SW_HIDE); break;
								case ID_TAB_STRINGEDITOR: ShowWindow(GetDlgItem(hTabs, ID_STRINGEDITOR), SW_HIDE); break;
								case ID_TAB_WAVEDITOR: ShowWindow(GetDlgItem(hTabs, ID_WAVEDITOR), SW_HIDE); break;
							}
						}
						break;
					default:
						return FALSE;
						break;
				}
			}
			break;

		default:
			return FALSE;
			break;
	}

	return TRUE;
}
