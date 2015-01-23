#ifndef TRIGGERS_H
#define TRIGGERS_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"
#include "TextTrigGenerator.h"

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
		void RefreshGroupList();
		void RefreshTrigList();

	private:
		u32 currTrigger;
		bool groupSelected[NUM_TRIG_PLAYERS];
		bool displayAll;
		u32 numVisibleTrigs;
		bool changeGroupHighlightOnly;

		ButtonControl buttonNew, buttonModify, buttonDelete, buttonCopy,
					  buttonMoveUp, buttonMoveDown, buttonMoveTo;

		ListBoxControl listGroups;
		ListBoxControl listTriggers;
		TextTrigGenerator tt;

		bool ShowTrigger(Trigger* trigger); // Checks if trigger should currently be shown
		void ClearGroups();
		string GetConditionString(u8 conditionNum, Trigger* trigger);
		string GetActionString(u8 actionNum, Trigger* trigger);
		string GetTriggerString(u32 trigNum, Trigger* trigger);
		bool GetTriggerDrawSize(HDC hDC, UINT &width, UINT &height, Scenario* chk, u32 triggerNum, Trigger* trigger);
		void DrawGroup(HDC hDC, RECT &rcItem, bool isSelected, u8 groupNum);
		void DrawTrigger(HDC hDC, RECT &rcItem, bool isSelected, Scenario* chk, u32 triggerNum, Trigger* trigger);
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif