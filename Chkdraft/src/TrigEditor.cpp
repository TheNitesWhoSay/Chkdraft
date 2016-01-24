#include "TrigEditor.h"
#include "Chkdraft.h"
#include "WindowsUI/WindowsUI.h"

enum ID {
	TAB_TRIGGERS = 0,
	TAB_TEMPLATES,
	TAB_COUNTERS,
	TAB_CUWPS,
	TAB_SWITCHES,

	WIN_NETTABTAB = ID_FIRST,
	WIN_TRIGGERS,
	WIN_TEMPLATES,
	WIN_COUNTERS,
	WIN_CUWPS,
	WIN_SWITCHES
};

TrigEditorWindow::TrigEditorWindow() : currTab(0)
{

}

bool TrigEditorWindow::CreateThis(HWND hParent)
{
	if ( getHandle() == NULL &&
		 ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_TRIGEDIT), hParent) )
	{
		ShowNormal();
		ChangeTab(currTab);
		return true;
	}
	else
	{
		ChangeTab(currTab);
		return false;
	}
	RefreshWindow();
}

bool TrigEditorWindow::DestroyThis()
{
	triggersWindow.trigModifyWindow.Hide();
	return ClassDialog::DestroyDialog();
}

void TrigEditorWindow::ChangeTab(u32 tabId)
{
	tabs.SetCurSel(tabId);

	tabs.HideTab(WIN_TRIGGERS);
	tabs.HideTab(WIN_TEMPLATES);
	tabs.HideTab(WIN_COUNTERS);
	tabs.HideTab(WIN_CUWPS);
	tabs.HideTab(WIN_SWITCHES);

	switch ( tabId )
	{
		case TAB_TRIGGERS : tabs.ShowTab(WIN_TRIGGERS ); break;
		case TAB_TEMPLATES: tabs.ShowTab(WIN_TEMPLATES); break;
		case TAB_COUNTERS : tabs.ShowTab(WIN_COUNTERS ); break;
		case TAB_CUWPS	  : tabs.ShowTab(WIN_CUWPS	  ); break;
		case TAB_SWITCHES : tabs.ShowTab(WIN_SWITCHES ); break;
	}

	currTab = tabId;
}

void TrigEditorWindow::RefreshWindow()
{
	triggersWindow.RefreshWindow(false);
	templatesWindow.RefreshWindow();
	countersWindow.RefreshWindow();
	cuwpsWindow.RefreshWindow(true);
	switchesWindow.RefreshWindow();
}

void TrigEditorWindow::CreateSubWindows(HWND hWnd)
{
	tabs.FindThis(hWnd, IDC_TRIGEDITTABS);
	const char* tabLabels[] = { "Triggers", "Templates", "Counters", "CUWPs", "Switches" };
	for ( int i=0; i<sizeof(tabLabels)/sizeof(const char*); i++ )
		tabs.InsertTab(i, tabLabels[i]);

	triggersWindow.CreateThis(tabs.getHandle(), WIN_TRIGGERS);
	templatesWindow.CreateThis(tabs.getHandle(), WIN_TEMPLATES);
	countersWindow.CreateThis(tabs.getHandle(), WIN_COUNTERS);
	cuwpsWindow.CreateThis(tabs.getHandle(), WIN_CUWPS);
	switchesWindow.CreateThis(tabs.getHandle(), WIN_SWITCHES);
	DoSize();
}

void TrigEditorWindow::DoSize()
{
	RECT rcCli;
	getClientRect(rcCli);
	tabs.SetPos(rcCli.left, rcCli.top, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top);
	tabs.getClientRect(rcCli);
	
	triggersWindow.SetPos(rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22);
	templatesWindow.SetPos(rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22);
	countersWindow.SetPos(rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22);
	cuwpsWindow.SetPos(rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22);
	switchesWindow.SetPos(rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22);

	triggersWindow.DoSize();
	templatesWindow.DoSize();
	countersWindow.DoSize();
	cuwpsWindow.DoSize();
	switchesWindow.DoSize();
}

BOOL TrigEditorWindow::DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
	switch ( nmhdr->code )
	{
	case TCN_SELCHANGE:
	{
		u32 selectedTab = tabs.GetCurSel();
		switch ( selectedTab )
		{
		case TAB_TRIGGERS: tabs.ShowTab(WIN_TRIGGERS); break;
		case TAB_TEMPLATES: tabs.ShowTab(WIN_TEMPLATES); break;
		case TAB_COUNTERS: tabs.ShowTab(WIN_COUNTERS); break;
		case TAB_CUWPS: tabs.ShowTab(WIN_CUWPS); break;
		case TAB_SWITCHES: tabs.ShowTab(WIN_SWITCHES); break;
		}
		currTab = selectedTab;
	}
	break;
	case TCN_SELCHANGING:
	{
		u32 selectedTab = tabs.GetCurSel();
		switch ( selectedTab )
		{
		case TAB_TRIGGERS: tabs.HideTab(WIN_TRIGGERS); break;
		case TAB_TEMPLATES: tabs.HideTab(WIN_TEMPLATES); break;
		case TAB_COUNTERS: tabs.HideTab(WIN_COUNTERS); break;
		case TAB_CUWPS: tabs.HideTab(WIN_CUWPS); break;
		case TAB_SWITCHES: tabs.HideTab(WIN_SWITCHES); break;
		}
	}
	break;
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

BOOL TrigEditorWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_ACTIVATE:
			if ( LOWORD(wParam) != WA_INACTIVE )
				chkd.SetCurrDialog(hWnd);
			return FALSE;
			break;

		case WM_INITDIALOG:
			SetSmallIcon((HANDLE)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PROGRAM_ICON), IMAGE_ICON, 16, 16, 0));
			CreateSubWindows(hWnd);
			ReplaceChildFonts(defaultFont);
			RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
			break;

		case WM_SIZE:
			DoSize();
			break;

		case WM_CLOSE:
			triggersWindow.trigModifyWindow.Hide();
			ClassDialog::DestroyDialog();
			return FALSE;
			break;

		default:
			return FALSE;
			break;
	}
	return TRUE;
}
