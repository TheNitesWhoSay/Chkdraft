#include "TrigModify.h"
#include "../../../Chkdraft.h"
#include <string>

enum_t(Id, u32, {
    WIN_GENERAL = ID_FIRST,
    WIN_PLAYERS,
    WIN_CONDITIONS,
    WIN_ACTIONS,
    WIN_TRIGMODIFYTEXT
});

#define NO_TRIGGER (u32(-1))

TrigModifyWindow::TrigModifyWindow() : currTab(Tab::General), trigIndex(NO_TRIGGER)
{

}

TrigModifyWindow::~TrigModifyWindow()
{

}

bool TrigModifyWindow::CreateThis(HWND hParent, u32 trigIndex)
{
    if ( getHandle() == NULL &&
         ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_TRIGMODIFY), hParent) )
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
    return ClassDialog::DestroyDialog();
}

void TrigModifyWindow::ChangeTab(Tab tab)
{
    tabs.SetCurSel((u32)tab);

    tabs.HideTab(Id::WIN_GENERAL);
    tabs.HideTab(Id::WIN_PLAYERS);
    tabs.HideTab(Id::WIN_CONDITIONS);
    tabs.HideTab(Id::WIN_ACTIONS);
    tabs.HideTab(Id::WIN_TRIGMODIFYTEXT);

    switch ( tab )
    {
        case Tab::General: tabs.ShowTab(Id::WIN_GENERAL); break;
        case Tab::Players: tabs.ShowTab(Id::WIN_PLAYERS); break;
        case Tab::Conditions: tabs.ShowTab(Id::WIN_CONDITIONS); break;
        case Tab::Actions: tabs.ShowTab(Id::WIN_ACTIONS); break;
        case Tab::Text: tabs.ShowTab(Id::WIN_TRIGMODIFYTEXT); break;
    }

    currTab = tab;
}

void TrigModifyWindow::RefreshWindow(u32 trigIndex)
{
    this->trigIndex = trigIndex;
    //Show();
    WindowsItem::SetWinText("Modify Trigger #" + std::to_string(trigIndex));
    generalWindow.RefreshWindow(trigIndex);
    playersWindow.RefreshWindow(trigIndex);
    conditionsWindow.RefreshWindow(trigIndex);
    actionsWindow.RefreshWindow(trigIndex);
    if ( currTab == Tab::Text )
        trigModifyTextWindow.RefreshWindow(trigIndex);
}

void TrigModifyWindow::CreateSubWindows(HWND hWnd)
{
    generalWindow.CreateThis(tabs.getHandle(), Id::WIN_GENERAL);
    playersWindow.CreateThis(tabs.getHandle(), Id::WIN_PLAYERS);
    conditionsWindow.CreateThis(tabs.getHandle(), Id::WIN_CONDITIONS);
    actionsWindow.CreateThis(tabs.getHandle(), Id::WIN_ACTIONS);
    trigModifyTextWindow.CreateThis(tabs.getHandle(), Id::WIN_TRIGMODIFYTEXT);
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
    return currTab == Tab::Text;
}

void TrigModifyWindow::RedrawThis()
{
    ClassDialog::RedrawThis();
    conditionsWindow.RedrawThis();
    actionsWindow.RedrawThis();
    generalWindow.RedrawThis();
    playersWindow.RedrawThis();
    trigModifyTextWindow.RedrawThis();
}

BOOL TrigModifyWindow::DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    switch ( nmhdr->code )
    {
    case TCN_SELCHANGE:
    {
        Tab selectedTab = (Tab)tabs.GetCurSel();
        switch ( selectedTab )
        {
            case Tab::General: tabs.ShowTab(Id::WIN_GENERAL); break;
            case Tab::Players: tabs.ShowTab(Id::WIN_PLAYERS); break;
            case Tab::Conditions: tabs.ShowTab(Id::WIN_CONDITIONS); break;
            case Tab::Actions: tabs.ShowTab(Id::WIN_ACTIONS); break;
            case Tab::Text: tabs.ShowTab(Id::WIN_TRIGMODIFYTEXT); break;
        }
        currTab = selectedTab;
    }
    break;
    case TCN_SELCHANGING:
    {
        Tab selectedTab = (Tab)tabs.GetCurSel();
        switch ( selectedTab )
        {
            case Tab::General: tabs.HideTab(Id::WIN_GENERAL); break;
            case Tab::Players: tabs.HideTab(Id::WIN_PLAYERS); break;
            case Tab::Conditions: tabs.HideTab(Id::WIN_CONDITIONS); break;
            case Tab::Actions: tabs.HideTab(Id::WIN_ACTIONS); break;
            case Tab::Text: tabs.HideTab(Id::WIN_TRIGMODIFYTEXT); break;
        }
    }
    break;
    default:
        return FALSE;
        break;
    }
    return TRUE;
}

BOOL TrigModifyWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_SHOWWINDOW:
            if ( wParam == FALSE && onTrigTextTab() )
                trigModifyTextWindow.ParentHidden();
            else if ( wParam == FALSE && currTab == Tab::Conditions )
                conditionsWindow.HideSuggestions();
            return FALSE;
            break;

        case WM_ACTIVATE:
            if ( LOWORD(wParam) != WA_INACTIVE )
                chkd.SetCurrDialog(hWnd);
            else // LOWORD(wParam) == WA_INACTIVE
                conditionsWindow.HideSuggestions();
            return FALSE;
            break;

        case WM_NCACTIVATE:
            if ( (BOOL)wParam == FALSE )
                conditionsWindow.HideSuggestions();
            return FALSE;
            break;

        case WM_INITDIALOG:
            {
                SetSmallIcon((HANDLE)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PROGRAM_ICON), IMAGE_ICON, 16, 16, 0 ));
                tabs.FindThis(hWnd, IDC_TRIGMODIFYTABS);
                const std::vector<std::string> tabLabels = { "General", "Players", "Conditions", "Actions", "Text" };
                for ( size_t i=0; i<tabLabels.size(); i++ )
                    tabs.InsertTab((u32)i, tabLabels[i]);
                CreateSubWindows(hWnd);
                DoSize();
                ReplaceChildFonts(defaultFont);
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
