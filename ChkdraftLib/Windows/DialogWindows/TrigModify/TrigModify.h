#ifndef TRIGMODIFY_H
#define TRIGMODIFY_H
#include "../../../../WindowsLib/WindowsUi.h"
#include "TrigGeneral.h"
#include "TrigPlayers.h"
#include "TrigConditions.h"
#include "TrigActions.h"
#include "TrigModifyText.h"

class TrigModifyWindow : public WinLib::ClassDialog
{
    public:
        enum class Tab {
            General,
            Players,
            Conditions,
            Actions,
            Text
        };

        TrigConditionsWindow conditionsWindow;
        TrigActionsWindow actionsWindow;

        TrigModifyWindow();
        virtual ~TrigModifyWindow();
        bool CreateThis(HWND hParent, u32 trigIndex);
        bool DestroyThis();
        void ChangeTab(Tab tab);
        void RefreshWindow(u32 trigIndex);
        void DoSize();
        bool onTrigTextTab();
        virtual void RedrawThis();

    protected:
        void CreateSubWindows(HWND hWnd);
        BOOL DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        Tab currTab;
        u32 trigIndex;
        WinLib::TabControl tabs;

        TrigGeneralWindow generalWindow;
        TrigPlayersWindow playersWindow;
        TrigModifyTextWindow trigModifyTextWindow;
};

#endif