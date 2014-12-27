#include "TrigEditor.h"
#include "Chkdraft.h"

TrigEditorWindow::TrigEditorWindow() : currTab(0), hTabs(NULL)
{

}

bool TrigEditorWindow::CreateThis(HWND hParent)
{
	if ( getHandle() == NULL &&
		 ClassWindow::CreateModelessDialog(MAKEINTRESOURCE(IDD_TRIGEDIT), hParent) )
	{
		ShowWindow(getHandle(), SW_SHOWNORMAL);
		ChangeTab(currTab);
		return true;
	}
	else
	{
		ChangeTab(currTab);
		return false;
	}
}

bool TrigEditorWindow::DestroyThis()
{
	if ( ClassWindow::DestroyDialog() )
	{
		hTabs = NULL;
		return true;
	}
	else
		return false;
}

void TrigEditorWindow::ChangeTab(u32 tabId)
{
	TabCtrl_SetCurSel(hTabs, tabId);

	ShowWindow(GetDlgItem(hTabs, ID_TRIGGERS), SW_HIDE);
	ShowWindow(GetDlgItem(hTabs, ID_TEMPLATES), SW_HIDE);
	ShowWindow(GetDlgItem(hTabs, ID_COUNTERS), SW_HIDE);
	ShowWindow(GetDlgItem(hTabs, ID_CUWPS), SW_HIDE);
	ShowWindow(GetDlgItem(hTabs, ID_SWITCHES), SW_HIDE);

	switch ( tabId )
	{
		case ID_TAB_TRIGGERS: ShowWindow(GetDlgItem(hTabs, ID_TRIGGERS), SW_SHOW); break;
		case ID_TAB_TEMPLATES: ShowWindow(GetDlgItem(hTabs, ID_TEMPLATES), SW_SHOW); break;
		case ID_TAB_COUNTERS: ShowWindow(GetDlgItem(hTabs, ID_COUNTERS), SW_SHOW); break;
		case ID_TAB_CUWPS: ShowWindow(GetDlgItem(hTabs, ID_CUWPS), SW_SHOW); break;
		case ID_TAB_SWITCHES: ShowWindow(GetDlgItem(hTabs, ID_SWITCHES), SW_SHOW); break;
	}

	currTab = tabId;
}

void TrigEditorWindow::RefreshWindow()
{
	triggersWindow.RefreshWindow();
	templatesWindow.RefreshWindow();
	countersWindow.RefreshWindow();
	cuwpsWindow.RefreshWindow();
	switchesWindow.RefreshWindow();
}

void TrigEditorWindow::CreateSubWindows()
{
	triggersWindow.CreateThis(hTabs);
	templatesWindow.CreateThis(hTabs);
	countersWindow.CreateThis(hTabs);
	cuwpsWindow.CreateThis(hTabs);
	switchesWindow.CreateThis(hTabs);
	DoSize();
}

void TrigEditorWindow::DoSize()
{
	RECT rcCli;
	GetClientRect(getHandle(), &rcCli);
	MoveWindow(hTabs, rcCli.left, rcCli.top, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top, TRUE);	
	GetClientRect(hTabs, &rcCli);
	MoveWindow(triggersWindow.getHandle(), rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22, FALSE);
	MoveWindow(templatesWindow.getHandle(), rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22, FALSE);
	MoveWindow(countersWindow.getHandle(), rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22, FALSE);
	MoveWindow(cuwpsWindow.getHandle(), rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22, FALSE);
	MoveWindow(switchesWindow.getHandle(), rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22, FALSE);

	triggersWindow.DoSize();
	templatesWindow.DoSize();
	countersWindow.DoSize();
	cuwpsWindow.DoSize();
	switchesWindow.DoSize();
}

BOOL TrigEditorWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_ACTIVATE:
			if ( LOWORD(wParam) != WA_INACTIVE )
				chkd.SetCurrDialog(hWnd);
			break;

		case WM_INITDIALOG:
			{
				hTabs = GetDlgItem(hWnd, IDC_TRIGEDITTABS);
				CreateSubWindows();

				LPARAM icon = (LPARAM)LoadImage( GetModuleHandle(NULL),
												 MAKEINTRESOURCE(IDI_PROGRAM_ICON),
												 IMAGE_ICON, 16, 16, 0 );
				SendMessage(hTabs, WM_SETFONT, (WPARAM)defaultFont, FALSE);
				SendMessage(hWnd, WM_SETICON, ICON_SMALL, icon);

				const char* tabs[] = {
					"Triggers", "Templates", "Counters", "CUWPs", "Switches"
				};

				HBITMAP newTabImage = (HBITMAP)LoadImage( GetModuleHandle(NULL),
														  MAKEINTRESOURCE(IDB_NEWTAB),
														  IMAGE_BITMAP, 16, 16, 0 );
				HIMAGELIST hImages = ImageList_Create(16, 16, 0, 1, 1);
				ImageList_Add(hImages, newTabImage, NULL);
				TabCtrl_SetImageList(hTabs, hImages);
				TabCtrl_SetMinTabWidth(hTabs, 5);

				TCITEM item = { };
				item.mask = TCIF_TEXT;
				item.iImage = -1;
				
				for ( int i=0; i<sizeof(tabs)/sizeof(const char*); i++ )
				{
					item.pszText = (LPSTR)tabs[i];
					item.cchTextMax = strlen(tabs[i]);
					TabCtrl_InsertItem(hTabs, i, &item);
				}
				ReplaceChildFonts(defaultFont);
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
			}
			break;

		case WM_SIZE:
			DoSize();
			break;

		case WM_SIZING:
			return DefWindowProc(hWnd, msg, wParam, lParam);
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
								case ID_TAB_TRIGGERS: ShowWindow(GetDlgItem(hTabs, ID_TRIGGERS), SW_SHOW); break;
								case ID_TAB_TEMPLATES: ShowWindow(GetDlgItem(hTabs, ID_TEMPLATES), SW_SHOW); break;
								case ID_TAB_COUNTERS: ShowWindow(GetDlgItem(hTabs, ID_COUNTERS), SW_SHOW); break;
								case ID_TAB_CUWPS: ShowWindow(GetDlgItem(hTabs, ID_CUWPS), SW_SHOW); break;
								case ID_TAB_SWITCHES: ShowWindow(GetDlgItem(hTabs, ID_SWITCHES), SW_SHOW); break;
							}
							currTab = selectedTab;
						}
						break;
					case TCN_SELCHANGING:
						{
							u32 selectedTab = TabCtrl_GetCurSel(hTabs);
							switch ( selectedTab )
							{
								case ID_TAB_TRIGGERS: ShowWindow(GetDlgItem(hTabs, ID_TRIGGERS), SW_HIDE); break;
								case ID_TAB_TEMPLATES: ShowWindow(GetDlgItem(hTabs, ID_TEMPLATES), SW_HIDE); break;
								case ID_TAB_COUNTERS: ShowWindow(GetDlgItem(hTabs, ID_COUNTERS), SW_HIDE); break;
								case ID_TAB_CUWPS: ShowWindow(GetDlgItem(hTabs, ID_CUWPS), SW_HIDE); break;
								case ID_TAB_SWITCHES: ShowWindow(GetDlgItem(hTabs, ID_SWITCHES), SW_HIDE); break;
							}
						}
						break;
					default:
						return FALSE;
						break;
				}
			}
			break;

		case WM_CLOSE:
			ClassWindow::DestroyDialog();
			break;

		default:
			return FALSE;
			break;
	}
	return TRUE;
}