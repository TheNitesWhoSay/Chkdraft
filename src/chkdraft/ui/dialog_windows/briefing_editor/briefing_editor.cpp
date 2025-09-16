#include "briefing_editor.h"
#include "chkdraft/chkdraft.h"
#include <windows/windows_ui.h>

enum_t(Id, u32, {
    WIN_BRIEFNETTABTAB = ID_FIRST,
    WIN_BRIEFTRIGGERS,
    WIN_BRIEFTEMPLATES,
});

BriefingTrigEditorWindow::BriefingTrigEditorWindow() : currTab(Tab::BriefingTriggers)
{

}

BriefingTrigEditorWindow::~BriefingTrigEditorWindow()
{

}

bool BriefingTrigEditorWindow::CreateThis(HWND hParent)
{
    if ( getHandle() == NULL &&
         ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_BRIEFTRIGEDIT), hParent) )
    {
        ShowNormal();
        ChangeTab(currTab);
        return true;
    }
    else
    {
        ChangeTab(currTab);
        RefreshWindow();
        return false;
    }
}

bool BriefingTrigEditorWindow::DestroyThis()
{
    this->Hide();
    
    briefingTriggersWindow.briefingTrigModifyWindow.DestroyThis();
    briefingTriggersWindow.DestroyThis();
    briefingTemplatesWindow.DestroyThis();
    this->currTab = Tab::BriefingTriggers;
    return ClassDialog::DestroyDialog();
}

void BriefingTrigEditorWindow::ChangeTab(Tab tab)
{
    if ( tabs.GetCurSel() != u32(tab) )
    {
        tabs.SetCurSel((u32)tab);

        tabs.HideTab(Id::WIN_BRIEFTRIGGERS);
        tabs.HideTab(Id::WIN_BRIEFTEMPLATES);

        switch ( tab )
        {
            case Tab::BriefingTriggers  : tabs.ShowTab(Id::WIN_BRIEFTRIGGERS ); break;
            case Tab::BriefingTemplates : tabs.ShowTab(Id::WIN_BRIEFTEMPLATES); break;
        }

        currTab = tab;
    }
}

void BriefingTrigEditorWindow::RefreshWindow()
{
    if ( getHandle() == NULL )
        return;

    briefingTriggersWindow.RefreshWindow(false);
    briefingTemplatesWindow.RefreshWindow();
    ChangeTab(currTab);
}

void BriefingTrigEditorWindow::CreateSubWindows(HWND hWnd)
{
    tabs.FindThis(hWnd, IDC_BRIEFTRIGEDITTABS);
    const std::vector<std::string> tabLabels = { "Triggers", "Templates" };
    for ( size_t i=0; i<tabLabels.size(); i++ )
        tabs.InsertTab((u32)i, tabLabels[i]);

    briefingTriggersWindow.CreateThis(tabs.getHandle(), Id::WIN_BRIEFTRIGGERS);
    briefingTemplatesWindow.CreateThis(tabs.getHandle(), Id::WIN_BRIEFTEMPLATES);
    DoSize();
}

void BriefingTrigEditorWindow::DoSize()
{
    RECT rcCli;
    getClientRect(rcCli);
    tabs.SetPos(rcCli.left, rcCli.top, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top);
    tabs.getClientRect(rcCli);
    
    briefingTriggersWindow.SetPos(rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22);
    briefingTemplatesWindow.SetPos(rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22);

    briefingTriggersWindow.DoSize();
    briefingTemplatesWindow.DoSize();
}

BOOL BriefingTrigEditorWindow::DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    switch ( nmhdr->code )
    {
    case TCN_SELCHANGE:
    {
        Tab selectedTab = (Tab)tabs.GetCurSel();
        switch ( selectedTab )
        {
        case Tab::BriefingTriggers: tabs.ShowTab(Id::WIN_BRIEFTRIGGERS); break;
        case Tab::BriefingTemplates: tabs.ShowTab(Id::WIN_BRIEFTEMPLATES); break;
        }
        currTab = selectedTab;
    }
    break;
    case TCN_SELCHANGING:
    {
        chkd.briefingTrigEditorWindow->SetWinText("Briefing Trigger Editor");
        Tab selectedTab = (Tab)tabs.GetCurSel();
        switch ( selectedTab )
        {
        case Tab::BriefingTriggers: tabs.HideTab(Id::WIN_BRIEFTRIGGERS); break;
        case Tab::BriefingTemplates: tabs.HideTab(Id::WIN_BRIEFTEMPLATES); break;
        }
    }
    break;
    default:
        return FALSE;
        break;
    }
    return TRUE;
}

BOOL BriefingTrigEditorWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
