#ifndef TRIGGERS_H
#define TRIGGERS_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

// Max triggers (assuming 4gb sections are possible): 1,789,569 (2,400 bytes per trigger; 2^32 max size)

class TriggersWindow : public ClassWindow
{
	public:
		TriggersWindow();
		bool CreateThis(HWND hParent);
		bool DestroyThis();
		void RefreshWindow();
		void DoSize();

	protected:
		void CreateSubWindows(HWND hWnd);

	private:
		u32 currTrigger;
		u32 currGroups;
		ButtonControl buttonNew, buttonModify, buttonDelete, buttonCopy,
					  buttonMoveUp, buttonMoveDown, buttonMoveTo;

		ListBoxControl listGroups;
		ListBoxControl listTriggers;

		bool ShowTrigger(Trigger* trigger); // Checks if trigger should currently be shown
		string GetTriggerString(u32 trigNum);
		bool GetTriggerDrawSize(HDC hDC, UINT &width, UINT &height, Scenario* chk, u32 triggerNum, Trigger* trigger);
		void DrawTrigger(HDC hDC, RECT &rcItem, bool isSelected, Scenario* chk, u32 triggerNum, Trigger* trigger);
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif