#ifndef UNITPROPERTIES_H
#define UNITPROPERTIES_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"
#include "PreservedUnitStats.h"

class UnitWindow : public ClassWindow
{
	public:
		UnitWindow();
		bool CreateThis(HWND hParent);
		bool DestroyThis();
		void SetChangeHighlightOnly(bool changeHighlightOnly);
		void ChangeCurrOwner(HWND hUnitProperties, u8 newPlayer);
		void ChangeOwner(HWND hUnitList, int index, u8 newPlayer);

	protected:
		void EnableUnitEditing(HWND hWnd);
		void DisableUnitEditing(HWND hWnd);
		void SetUnitFieldText(HWND hWnd, ChkUnit* unit);
		void SwapIndexes(HWND hListView, LPARAM index1, LPARAM index2);
		void ChangeIndex(HWND hListView, LPARAM oldLParam, LPARAM newLParam);
		int CompareLvItems(LPARAM index1, LPARAM index2);
		BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		u16 columnSortedBy;
		bool flipSort;
		bool initilizing;
		bool changeHighlightOnly;
		PreservedUnitStats preservedStats;
};

#endif