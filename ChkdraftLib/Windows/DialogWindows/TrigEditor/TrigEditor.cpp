#include "TrigEditor.h"
#include "../../../Chkdraft.h"
#include "../../../../WindowsLib/WindowsUi.h"

enum_t(Id, u32, {
    WIN_NETTABTAB = ID_FIRST,
    WIN_TRIGGERS,
    WIN_TEMPLATES,
    WIN_COUNTERS,
    WIN_CUWPS,
    WIN_SWITCHES
});

TrigEditorWindow::TrigEditorWindow() : currTab(Tab::Triggers)
{

}

TrigEditorWindow::~TrigEditorWindow()
{

}

bool TrigEditorWindow::CreateThis(HWND hParent)
{
    if ( getHandle() == NULL &&
         ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_TRIGEDIT), hParent) )
    {
        ShowNormal();
        ChangeTab(currTab);
        RefreshWindow();
        return true;
    }
    else
    {
        ChangeTab(currTab);
        RefreshWindow();
        return false;
    }
}

bool TrigEditorWindow::DestroyThis()
{
    this->Hide();
    triggersWindow.trigModifyWindow.DestroyThis();
    triggersWindow.DestroyThis();
    templatesWindow.DestroyThis();
    countersWindow.DestroyThis();
    cuwpsWindow.DestroyThis();
    switchesWindow.DestroyThis();
    return ClassDialog::DestroyDialog();
}

void TrigEditorWindow::ChangeTab(Tab tab)
{
    tabs.SetCurSel((u32)tab);

    tabs.HideTab(Id::WIN_TRIGGERS);
    tabs.HideTab(Id::WIN_TEMPLATES);
    tabs.HideTab(Id::WIN_COUNTERS);
    tabs.HideTab(Id::WIN_CUWPS);
    tabs.HideTab(Id::WIN_SWITCHES);

    switch ( tab )
    {
        case Tab::Triggers  : tabs.ShowTab(Id::WIN_TRIGGERS ); break;
        case Tab::Templates : tabs.ShowTab(Id::WIN_TEMPLATES); break;
        case Tab::Counters  : tabs.ShowTab(Id::WIN_COUNTERS ); break;
        case Tab::Cuwps     : tabs.ShowTab(Id::WIN_CUWPS    ); break;
        case Tab::Switches  : tabs.ShowTab(Id::WIN_SWITCHES ); break;
    }

    currTab = tab;
}

void TrigEditorWindow::RefreshWindow()
{
    triggersWindow.RefreshWindow(false);
    templatesWindow.RefreshWindow();
    countersWindow.RefreshWindow();
    cuwpsWindow.RefreshWindow(true);
    switchesWindow.RefreshWindow();
    ChangeTab(currTab);
}

void TrigEditorWindow::CreateSubWindows(HWND hWnd)
{
    tabs.FindThis(hWnd, IDC_TRIGEDITTABS);
    const std::vector<std::string> tabLabels = { "Triggers", "Templates", "Counters", "CUWPs", "Switches" };
    for ( size_t i=0; i<tabLabels.size(); i++ )
        tabs.InsertTab((u32)i, tabLabels[i]);

    triggersWindow.CreateThis(tabs.getHandle(), Id::WIN_TRIGGERS);
    templatesWindow.CreateThis(tabs.getHandle(), Id::WIN_TEMPLATES);
    countersWindow.CreateThis(tabs.getHandle(), Id::WIN_COUNTERS);
    cuwpsWindow.CreateThis(tabs.getHandle(), Id::WIN_CUWPS);
    switchesWindow.CreateThis(tabs.getHandle(), Id::WIN_SWITCHES);
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
        Tab selectedTab = (Tab)tabs.GetCurSel();
        switch ( selectedTab )
        {
        case Tab::Triggers: tabs.ShowTab(Id::WIN_TRIGGERS); break;
        case Tab::Templates: tabs.ShowTab(Id::WIN_TEMPLATES); break;
        case Tab::Counters: tabs.ShowTab(Id::WIN_COUNTERS); break;
        case Tab::Cuwps: tabs.ShowTab(Id::WIN_CUWPS); break;
        case Tab::Switches: tabs.ShowTab(Id::WIN_SWITCHES); break;
        }
        currTab = selectedTab;
    }
    break;
    case TCN_SELCHANGING:
    {
        chkd.trigEditorWindow.SetWinText("Trigger Editor");
        Tab selectedTab = (Tab)tabs.GetCurSel();
        switch ( selectedTab )
        {
        case Tab::Triggers: tabs.HideTab(Id::WIN_TRIGGERS); break;
        case Tab::Templates: tabs.HideTab(Id::WIN_TEMPLATES); break;
        case Tab::Counters: tabs.HideTab(Id::WIN_COUNTERS); break;
        case Tab::Cuwps: tabs.HideTab(Id::WIN_CUWPS); break;
        case Tab::Switches: tabs.HideTab(Id::WIN_SWITCHES); break;
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
            SetSmallIcon(WinLib::ResourceManager::getIcon(IDI_PROGRAM_ICON, 16, 16));
            CreateSubWindows(hWnd);
            defaultChildFonts();
            RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
            break;

        case WM_SIZE:
            DoSize();
            break;

        case WM_CLOSE:
            DestroyThis();
            return FALSE;
            break;

        default:
            return FALSE;
            break;
    }
    return TRUE;
}
