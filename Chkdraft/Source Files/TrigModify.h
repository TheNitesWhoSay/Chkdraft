#ifndef TRIGMODIFY_H
#define TRIGMODIFY_H
#include "Windows UI/WindowsUI.h"
#include "TrigMeta.h"
#include "TrigPlayers.h"
#include "TrigConditions.h"
#include "TrigActions.h"
#include "TrigModifyText.h"

class TrigModifyWindow : public ClassWindow
{
	public:
		TrigModifyWindow();
		bool CreateThis(HWND hParent, u32 trigIndex);
		bool DestroyThis();
		void ChangeTab(u32 tabId);
		void RefreshWindow(u32 trigIndex);
		void DoSize();

	protected:
		void CreateSubWindows(HWND hWnd);

	private:
		u32 currTab;
		u32 trigIndex;
		TabControl tabs;

		TrigMetaWindow metaWindow;
		TrigPlayersWindow playersWindow;
		TrigConditionsWindow conditionsWindow;
		TrigActionsWindow actionsWindow;
		TrigModifyTextWindow trigModifyTextWindow;

		BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif