#ifndef LISTBOXCONTROL_H
#define LISTBOXCONTROL_H
#include "WindowControl.h"
#include <queue>

enum LB
{
	/** Sent to the parent when an item is double clicked
		The WPARAM specifies the item index that was double clicked
		The LPARAM is the list box's handle */
	WM_DBLCLKITEM = (WM_APP+1), 

	/** Sent to the parent before several items are measured
		The WPARAM is NULL
		The LPARAM is the list box's handle */
	WM_PREMEASUREITEMS,

	/** Sent to the parent after several items are measured
		The WPARAM is NULL
		The LPARAM is the list box's handle */
	WM_POSTMEASUREITEMS,

	/** Sent to the parent before several items are drawn
		The WPARAM is NULL
		The LPARAM is the list box's handle */
	WM_PREDRAWITEMS,

	/** Sent to the parent after several items are drawn
		The WPARAM is NULL
		The LPARAM is the list box's handle */
	WM_POSTDRAWITEMS
};

class ListBoxControl : public WindowControl
{
	public:
		ListBoxControl();
		~ListBoxControl();

		// Attempts to create a list box
		bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool ownerDrawn, bool multiColumn, u32 id);
		void ClearItems();
		int AddItem(u32 item);
		int AddString(const char* str);
		bool InsertString(int index, const char* str);
		bool InsertItem(int index, u32 item);
		bool ClearSel();
		bool SetCurSel(int index); // Used with single-selection list boxes
		bool SelectItem(int index); // Used with multi-selection list boxes
		bool SetItemData(int index, u32 data);
		bool SetItemHeight(int index, int height);
		bool RemoveItem(int index);
		bool SetTopIndex(int index);

		int GetNumItems();
		int ItemHeight();
		bool GetItemHeight(int index, int &height);
		int ColumnWidth();
		bool GetCurSel(int &sel); // Used with single-selection list boxes
		int GetNumSel(); // Gets the number of items selected (in a multi-selection list box)
		bool GetSelString(int n, std::string &str); // Attempts to get the nth selected item's string (in a multi-selection list box)
		bool GetSelItem(int n, int &itemData); // Attempts to get the nth selected item's data (in a multi-selection list box)
		bool GetItemData(int index, u32 &data);
		int GetTopIndex();
		virtual LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		bool autoRedraw;
		std::queue<u32> itemsToAdd;
};

#endif