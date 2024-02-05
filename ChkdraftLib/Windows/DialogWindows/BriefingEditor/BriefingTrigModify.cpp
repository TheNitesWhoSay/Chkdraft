#include "BriefingTrigModify.h"
#include "../../../Chkdraft.h"
#include <string>

enum_t(Id, u32, {
    WIN_BRIEFPLAYERS = ID_FIRST,
    WIN_BRIEFACTIONS,
    WIN_BRIEFTRIGMODIFYTEXT
});

#define NO_TRIGGER (u32(-1))

BriefingTrigModifyWindow::BriefingTrigModifyWindow() : currTab(Tab::Players), briefingTrigIndex(NO_TRIGGER)
{

}

BriefingTrigModifyWindow::~BriefingTrigModifyWindow()
{

}

bool BriefingTrigModifyWindow::CreateThis(HWND hParent, u32 briefingTrigIndex)
{
    if ( getHandle() == NULL &&
         ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_TRIGMODIFY), hParent) )
    {
        ShowNormal();
        ChangeTab(currTab);
        RefreshWindow(briefingTrigIndex);
        return true;
    }
    else if ( getHandle() != NULL )
    {
        Show();
        ChangeTab(currTab);
        RefreshWindow(briefingTrigIndex);
    }
    return false;
}

bool BriefingTrigModifyWindow::DestroyThis()
{
    ClassDialog::Hide();
    playersWindow.DestroyThis();
    briefingActionsWindow.DestroyThis();
    briefingTrigModifyTextWindow.DestroyThis();
    this->briefingTrigIndex = NO_TRIGGER;
    this->currTab = Tab::Players;
    return ClassDialog::DestroyDialog();
}

void BriefingTrigModifyWindow::ChangeTab(Tab tab)
{
    tabs.SetCurSel((u32)tab);

    tabs.HideTab(Id::WIN_BRIEFPLAYERS);
    tabs.HideTab(Id::WIN_BRIEFACTIONS);
    tabs.HideTab(Id::WIN_BRIEFTRIGMODIFYTEXT);

    switch ( tab )
    {
        case Tab::Players: tabs.ShowTab(Id::WIN_BRIEFPLAYERS); break;
        case Tab::Actions: tabs.ShowTab(Id::WIN_BRIEFACTIONS); break;
        case Tab::Text: tabs.ShowTab(Id::WIN_BRIEFTRIGMODIFYTEXT); break;
    }

    currTab = tab;
}

void BriefingTrigModifyWindow::RefreshWindow(u32 briefingTrigIndex)
{
    this->briefingTrigIndex = briefingTrigIndex;
    WindowsItem::SetWinText("Modify Briefing Trigger #" + std::to_string(briefingTrigIndex));
    playersWindow.RefreshWindow(briefingTrigIndex);
    briefingActionsWindow.RefreshWindow(briefingTrigIndex);
    if ( currTab == Tab::Text )
        briefingTrigModifyTextWindow.RefreshWindow(briefingTrigIndex);
    else
        briefingTrigModifyTextWindow.SetTrigIndex(briefingTrigIndex);
}

void BriefingTrigModifyWindow::CreateSubWindows(HWND hWnd)
{
    playersWindow.CreateThis(tabs.getHandle(), Id::WIN_BRIEFPLAYERS);
    briefingActionsWindow.CreateThis(tabs.getHandle(), Id::WIN_BRIEFACTIONS);
    briefingTrigModifyTextWindow.CreateThis(tabs.getHandle(), Id::WIN_BRIEFTRIGMODIFYTEXT);
    DoSize();
}

void BriefingTrigModifyWindow::DoSize()
{
    RECT rcCli;
    if ( getClientRect(rcCli) )
    {
        tabs.SetPos(rcCli.left, rcCli.top, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top);
        if ( tabs.getClientRect(rcCli) )
        {
            playersWindow.SetPos(rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22);
            briefingActionsWindow.SetPos(rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22);
            briefingTrigModifyTextWindow.SetPos(rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22);

            playersWindow.DoSize();
            briefingActionsWindow.DoSize();
            briefingTrigModifyTextWindow.DoSize();
        }
    }
}

bool BriefingTrigModifyWindow::onBriefingTrigTextTab()
{
    return currTab == Tab::Text;
}

void BriefingTrigModifyWindow::RedrawThis()
{
    ClassDialog::RedrawThis();
    playersWindow.RedrawThis();
    briefingActionsWindow.RedrawThis();
    briefingTrigModifyTextWindow.RedrawThis();
}

BOOL BriefingTrigModifyWindow::DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    switch ( nmhdr->code )
    {
    case TCN_SELCHANGE:
    {
        Tab selectedTab = (Tab)tabs.GetCurSel();
        switch ( selectedTab )
        {
            case Tab::Players: tabs.ShowTab(Id::WIN_BRIEFPLAYERS); break;
            case Tab::Actions: tabs.ShowTab(Id::WIN_BRIEFACTIONS); break;
            case Tab::Text: tabs.ShowTab(Id::WIN_BRIEFTRIGMODIFYTEXT); break;
        }
        currTab = selectedTab;
    }
    break;
    case TCN_SELCHANGING:
    {
        Tab selectedTab = (Tab)tabs.GetCurSel();
        switch ( selectedTab )
        {
            case Tab::Players: tabs.HideTab(Id::WIN_BRIEFPLAYERS); break;
            case Tab::Actions: tabs.HideTab(Id::WIN_BRIEFACTIONS); break;
            case Tab::Text: tabs.HideTab(Id::WIN_BRIEFTRIGMODIFYTEXT); break;
        }
    }
    break;
    default:
        return FALSE;
        break;
    }
    return TRUE;
}

BOOL BriefingTrigModifyWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_SHOWWINDOW:
            if ( wParam == FALSE && onBriefingTrigTextTab() )
                briefingTrigModifyTextWindow.ParentHidden();
            else if ( wParam == FALSE && currTab == Tab::Actions )
                briefingActionsWindow.HideSuggestions();
            return FALSE;
            break;

        case WM_ACTIVATE:
            if ( LOWORD(wParam) != WA_INACTIVE )
                chkd.SetCurrDialog(hWnd);
            else // LOWORD(wParam) == WA_INACTIVE
            {
                if ( !briefingActionsWindow.IsSuggestionsWindow((HWND)lParam) )
                    briefingActionsWindow.HideSuggestions();
            }
            return FALSE;
            break;

        case WM_NCACTIVATE:
            if ( (BOOL)wParam == FALSE && !briefingActionsWindow.IsSuggestionsWindow((HWND)lParam) )
                briefingActionsWindow.HideSuggestions();
            return FALSE;
            break;

        case WM_INITDIALOG:
            {
                SetSmallIcon(WinLib::ResourceManager::getIcon(IDI_PROGRAM_ICON, 16, 16));
                tabs.FindThis(hWnd, IDC_TRIGMODIFYTABS);
                const std::vector<std::string> tabLabels = { "Players", "Actions", "Text" };
                for ( size_t i=0; i<tabLabels.size(); i++ )
                    tabs.InsertTab((u32)i, tabLabels[i]);
                CreateSubWindows(hWnd);
                DoSize();
                defaultChildFonts();
                RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
            }
            break;

        case WM_SIZE:
            DoSize();
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
