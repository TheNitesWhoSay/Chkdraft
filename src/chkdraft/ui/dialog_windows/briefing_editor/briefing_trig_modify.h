#ifndef BRIEFINGTRIGMODIFY_H
#define BRIEFINGTRIGMODIFY_H
#include "../../../../WindowsLib/WindowsUi.h"
#include "BriefingTrigPlayers.h"
#include "BriefingTrigActions.h"
#include "BriefingTrigModifyText.h"

class BriefingTrigModifyWindow : public WinLib::ClassDialog
{
    public:
        enum class Tab {
            Players,
            Actions,
            Text
        };

        BriefingTrigActionsWindow briefingActionsWindow;

        BriefingTrigModifyWindow();
        virtual ~BriefingTrigModifyWindow();
        bool CreateThis(HWND hParent, u32 briefingTrigIndex);
        bool DestroyThis();
        void ChangeTab(Tab tab);
        void RefreshWindow(u32 briefingTrigIndex);
        void DoSize();
        bool onBriefingTrigTextTab();
        virtual void RedrawThis();

    protected:
        void CreateSubWindows(HWND hWnd);
        BOOL DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        Tab currTab;
        u32 briefingTrigIndex;
        WinLib::TabControl tabs;

        BriefingTrigPlayersWindow playersWindow;
        BriefingTrigModifyTextWindow briefingTrigModifyTextWindow;
};

#endif