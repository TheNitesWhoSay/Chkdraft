#ifndef TRIGMODIFY_H
#define TRIGMODIFY_H
#include "WindowsUI/WindowsUI.h"
#include "TrigGeneral.h"
#include "TrigPlayers.h"
#include "TrigConditions.h"
#include "TrigActions.h"
#include "TrigModifyText.h"

class TrigModifyWindow : public ClassDialog
{
    public:
        TrigConditionsWindow conditionsWindow;
        TrigActionsWindow actionsWindow;

        TrigModifyWindow();
        bool CreateThis(HWND hParent, u32 trigIndex);
        bool DestroyThis();
        void ChangeTab(u32 tabId);
        void RefreshWindow(u32 trigIndex);
        void DoSize();
        bool onTrigTextTab();
        virtual void RedrawThis();

    protected:
        void CreateSubWindows(HWND hWnd);
        BOOL DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        u32 currTab;
        u32 trigIndex;
        TabControl tabs;

        TrigGeneralWindow generalWindow;
        TrigPlayersWindow playersWindow;
        TrigModifyTextWindow trigModifyTextWindow;
};

#endif