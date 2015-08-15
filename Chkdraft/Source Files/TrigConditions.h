#ifndef TRIGCONDITIONS_H
#define TRIGCONDITIONS_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class TrigConditionsWindow : public ClassWindow
{
	public:
		TrigConditionsWindow();
		bool CreateThis(HWND hParent, u32 windowId);
		bool DestroyThis();
		void RefreshWindow(u32 trigIndex);
		void DoSize();
		void ProcessKeyDown(WPARAM wParam, LPARAM lParam);

	protected:
		void InitializeArgMaps(); // Gives default values to all the argMaps
		void CreateSubWindows(HWND hWnd);
		void OnLeave();

	private:
		GridViewControl listConditions;
		EditControl editListItem;
		HBRUSH hBlack;
		u32 trigIndex;

		std::vector<u8> conditionArgMaps[16];
		std::vector<u8> actionArgMaps[64];

		LRESULT Notify(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT MeasureItem(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT EraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		BOOL GridItemChanging(u16 gridItemX, u16 gridItemY, string& str);
		void DrawSelectedCondition();
		int GetGridItemWidth(int gridItemX, int gridItemY);
		void PreDrawItems();
		void SysColorRect(HDC hDC, RECT &rect, DWORD color);
		void DrawItemBackground(HDC hDC, int gridItemX, int gridItemY, RECT &rcItem, int width, int xStart);
		void DrawItemFrame(HDC hDC, RECT &rcItem, int width, int &xStart);
		void DrawGridViewItem(HDC hDC, int gridItemX, int gridItemY, RECT &rcItem, int &xStart);
		void DrawGridViewRow(UINT gridId, PDRAWITEMSTRUCT pdis);
		void DrawTouchups(HDC hDC);
		void PostDrawItems();
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif