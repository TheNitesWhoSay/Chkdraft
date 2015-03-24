#include "TrigModify.h"
#include "Chkdraft.h"

enum ID {
	TAB_GENERAL = 0,
	TAB_PLAYERS,
	TAB_CONDITIONS,
	TAB_ACTIONS,
	TAB_TEXT,

	WIN_GENERAL = ID_FIRST,
	WIN_PLAYERS,
	WIN_CONDITIONS,
	WIN_ACTIONS,
	WIN_TRIGMODIFYTEXT
};

#define NO_TRIGGER (u32(-1))

TrigModifyWindow::TrigModifyWindow() : currTab(0), trigIndex(NO_TRIGGER)
{

}

bool TrigModifyWindow::CreateThis(HWND hParent, u32 trigIndex)
{	
	if ( getHandle() == NULL &&
		 ClassWindow::CreateModelessDialog(MAKEINTRESOURCE(IDD_TRIGMODIFY), hParent) )
	{
		ShowNormal();
		ChangeTab(currTab);
		RefreshWindow(trigIndex);
		return true;
	}
	else if ( getHandle() != NULL )
	{
		Show();
		ChangeTab(currTab);
		RefreshWindow(trigIndex);
	}
	return false;
}

bool TrigModifyWindow::DestroyThis()
{
	return ClassWindow::DestroyDialog();
}

void TrigModifyWindow::ChangeTab(u32 tabId)
{
	tabs.SetCurSel(tabId);

	tabs.HideTab(WIN_GENERAL);
	tabs.HideTab(WIN_PLAYERS);
	tabs.HideTab(WIN_CONDITIONS);
	tabs.HideTab(WIN_ACTIONS);
	tabs.HideTab(WIN_TRIGMODIFYTEXT);

	switch ( tabId )
	{
		case TAB_GENERAL: tabs.ShowTab(WIN_GENERAL); break;
		case TAB_PLAYERS: tabs.ShowTab(WIN_PLAYERS); break;
		case TAB_CONDITIONS: tabs.ShowTab(WIN_CONDITIONS); break;
		case TAB_ACTIONS: tabs.ShowTab(WIN_ACTIONS); break;
		case TAB_TEXT: tabs.ShowTab(WIN_TRIGMODIFYTEXT); break;
	}

	currTab = tabId;
}

void TrigModifyWindow::RefreshWindow(u32 trigIndex)
{
	this->trigIndex = trigIndex;
	Show();
	SetTitle((string("Modify Trigger #") + std::to_string(trigIndex)).c_str());
	generalWindow.RefreshWindow(trigIndex);
	playersWindow.RefreshWindow();
	conditionsWindow.RefreshWindow();
	actionsWindow.RefreshWindow();
	trigModifyTextWindow.RefreshWindow(trigIndex);
}

void TrigModifyWindow::CreateSubWindows(HWND hWnd)
{
	generalWindow.CreateThis(tabs.getHandle(), WIN_GENERAL);
	playersWindow.CreateThis(tabs.getHandle(), WIN_PLAYERS);
	conditionsWindow.CreateThis(tabs.getHandle(), WIN_CONDITIONS);
	actionsWindow.CreateThis(tabs.getHandle(), WIN_ACTIONS);
	trigModifyTextWindow.CreateThis(tabs.getHandle(), WIN_TRIGMODIFYTEXT);
	DoSize();
}

void TrigModifyWindow::DoSize()
{
	RECT rcCli;
	if ( getClientRect(rcCli) )
	{
		tabs.SetPos(rcCli.left, rcCli.top, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top);
		if ( tabs.getClientRect(rcCli) )
		{
			generalWindow.SetPos(rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22);
			playersWindow.SetPos(rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22);
			conditionsWindow.SetPos(rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22);
			actionsWindow.SetPos(rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22);
			trigModifyTextWindow.SetPos(rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22);

			generalWindow.DoSize();
			playersWindow.DoSize();
			conditionsWindow.DoSize();
			actionsWindow.DoSize();
			trigModifyTextWindow.DoSize();
		}
	}
}

bool TrigModifyWindow::onTrigTextTab()
{
	return currTab == TAB_TEXT;
}

BOOL TrigModifyWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_SHOWWINDOW:
			if ( wParam == false && onTrigTextTab() )
				trigModifyTextWindow.ParentHidden();
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;

		case WM_ACTIVATE:
			if ( LOWORD(wParam) != WA_INACTIVE )
				chkd.SetCurrDialog(hWnd);
			break;

		case WM_INITDIALOG:
			{
				SetSmallIcon((HANDLE)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PROGRAM_ICON), IMAGE_ICON, 16, 16, 0 ));
				tabs.FindThis(hWnd, IDC_TRIGMODIFYTABS);
				const char* tabLabels[] = { "General", "Players", "Conditions", "Actions", "Text" };
				for ( int i=0; i<sizeof(tabLabels)/sizeof(const char*); i++ )
					tabs.InsertTab(i, tabLabels[i]);
				CreateSubWindows(hWnd);
				DoSize();
				ReplaceChildFonts(defaultFont);
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
			}
			break;

		case WM_SIZE:
			DoSize();
			break;

		case WM_NOTIFY:
			{
				NMHDR* notification = (NMHDR*)lParam;
				switch ( notification->code )
				{
					case TCN_SELCHANGE:
						{
							u32 selectedTab = tabs.GetCurSel();
							switch ( selectedTab )
							{
								case TAB_GENERAL: tabs.ShowTab(WIN_GENERAL); break;
								case TAB_PLAYERS: tabs.ShowTab(WIN_PLAYERS);break;
								case TAB_CONDITIONS: tabs.ShowTab(WIN_CONDITIONS); break;
								case TAB_ACTIONS: tabs.ShowTab(WIN_ACTIONS); break;
								case TAB_TEXT: tabs.ShowTab(WIN_TRIGMODIFYTEXT); break;
							}
							currTab = selectedTab;
						}
						break;
					case TCN_SELCHANGING:
						{
							u32 selectedTab = tabs.GetCurSel();
							switch ( selectedTab )
							{
								case TAB_GENERAL: tabs.HideTab(WIN_GENERAL); break;
								case TAB_PLAYERS: tabs.HideTab(WIN_PLAYERS); break;
								case TAB_CONDITIONS: tabs.HideTab(WIN_CONDITIONS); break;
								case TAB_ACTIONS: tabs.HideTab(WIN_ACTIONS); break;
								case TAB_TEXT: tabs.HideTab(WIN_TRIGMODIFYTEXT); break;
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
			ShowWindow(hWnd, SW_HIDE);
			break;

		default:
			return FALSE;
			break;
	}
	return TRUE;
}
