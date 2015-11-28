#ifndef TRIGCONDITIONS_H
#define TRIGCONDITIONS_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"
#include "Suggestions.h"
#include "ConditionsGrid.h"

#include <string>
#include <vector>

class TrigConditionsWindow : public ClassWindow, IConditionGridUser
{
	public:
		TrigConditionsWindow();
		bool CreateThis(HWND hParent, u32 windowId);
		bool DestroyThis();
		void RefreshWindow(u32 trigIndex);
		void DoSize();
		void ProcessKeyDown(WPARAM wParam, LPARAM lParam);
		void HideSuggestions();
		void ConditionEnableToggled(u8 conditionNum);

	protected:
		void InitializeArgMaps(); // Gives default values to all the argMaps
		void CreateSubWindows(HWND hWnd);
		void OnLeave();
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		ConditionsGrid listConditions;
		EditControl editListItem;
		HBRUSH hBlack;
		u32 trigIndex;

		std::vector<u8> conditionArgMaps[24];
		std::vector<u8> actionArgMaps[64];

		DropdownControl dropUnits;
		Suggestions &suggestions;

		LRESULT MeasureItem(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT EraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		bool TransformCondition(Condition &condition, u8 newId);
		BOOL GridItemChanging(u16 gridItemX, u16 gridItemY, std::string& str);
		void DrawSelectedCondition();
		int GetGridItemWidth(int gridItemX, int gridItemY);
		void CheckEnabledClicked(int conditionNum);

		void PreDrawItems();
		void SysColorRect(HDC hDC, RECT &rect, DWORD color);
		void DrawItemBackground(HDC hDC, int gridItemX, int gridItemY, RECT &rcItem, int width, int xStart);
		void DrawItemFrame(HDC hDC, RECT &rcItem, int width, int &xStart);
		void DrawGridViewItem(HDC hDC, int gridItemX, int gridItemY, RECT &rcItem, int &xStart);
		void DrawGridViewRow(UINT gridId, PDRAWITEMSTRUCT pdis);
		void DrawTouchups(HDC hDC);
		void PostDrawItems();

		void SuggestNothing();
		void SuggestUnit();
		void SuggestLocation();
		void SuggestPlayer();
		void SuggestAmount();
		void SuggestNumericComparison();
		void SuggestResourceType();
		void SuggestScoreType();
		void SuggestSwitch();
		void SuggestSwitchState();
		void SuggestComparison();
		void SuggestConditionType();
		void SuggestTypeIndex();
		void SuggestFlags();
		void SuggestInternalData();

		void GridEditStart(u16 gridItemX, u16 gridItemY);
		void NewSuggestion(std::string &str);

		void Activate(WPARAM wParam, LPARAM lParam);
		LRESULT ShowWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif