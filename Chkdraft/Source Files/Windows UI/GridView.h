#ifndef GRIDVIEW_H
#define GRIDVIEW_H
#include "ListViewControl.h"
#include "GridItem.h"
#include "EditControl.h"

/** A modified ListView for working with items as
	if they were individual units within an x/y grid
	may use some or all of the GV:: messages from
	Enumerations.h */

class GridViewControl : public ListViewControl
{
	public:
		GridViewControl();
		GridControlItem& item(int x, int y);

		bool CreateThis(HWND hParent, int x, int y, int width, int height, bool editable, u32 id);
		bool AddColumn(int insertAt, const char* title, int width, int alignmentFlags);
		void AddRow(int numColumns, LPARAM lParam);
		void RemoveRow(int rowNum);
		void DeselectAll();
		void ClearItems();
		bool DeleteAllItems();
		void DeleteSelection();
		void SelectRow(int index);
		void SelectSquareTo(int x, int y);
		void SelectSquare(int xStart, int yStart, int xEnd, int yEnd);
		void FocusItem(int x, int y);
		void EndEditing();
		void AutoSizeColumns(int minWidth);

		bool isEditing(int x, int y);
		bool isFocused(int x, int y);
		bool GetFocusedItem(int &x, int &y);
		bool GetFocusedItemRect(RECT &rect);
		bool GetEditItemRect(RECT &rect);
		
		// Checks where a point is in relation to the GridView items
		bool contentHitTest(int x, int y, bool &outsideLeft, bool &outsideTop, bool &outsideRight, bool &outsideBottom);
	
	protected:
		bool resize(int numRows, int numColumns);
		void AutoSizeColumn(int x, int minWidth);
		void DragSelectTo(int x, int y);
		void StartEditing(int xClick, int yClick, char initChar); // Pass -1, -1 if starting via typing, '\0' if by click
		void UpdateCaretPos(int xClick, int yClick);
		void SetCaretPos(int newCaretPos);
	
	private:
		static GridControlItem nullItem; // Used to prevent null reference errors
		GridControlItem** gridItems/*[y][x]*/;
		int numRows;
		int numColumns;
		int focusedX;
		int focusedY;
		bool lButtonDown;
		bool editing;
		int caretPos;
		EditControl editBox;
		bool ending;

		void LButtonDown(int xClick, int yClick);
		void LButtonDblClk(int xClick, int yClick);
		void MouseMove(int xMove, int yMove, WPARAM wParam);
		void MouseHover(int xHover, int yHover);
		void LButtonUp(int xRelease, int yRelease);
		void KeyDown(WPARAM wParam);
		void Paint(HWND hWnd);
		void DrawItems(HWND hWnd);
		LRESULT Notify(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif