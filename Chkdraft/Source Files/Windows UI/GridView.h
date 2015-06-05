#ifndef GRIDVIEW_H
#define GRIDVIEW_H
#include "ListViewControl.h"
#include "GridItem.h"

/** A modified ListView for working with items as
	if they were individual units within an x/y grid */

class GridViewControl : public ListViewControl
{
	public:
		GridViewControl();
		GridControlItem& item(int x, int y);

		bool CreateThis(HWND hParent, int x, int y, int width, int height, bool editable, u32 id);
		void AddColumn(int insertAt, const char* title, int width, int alignmentFlags);
		void AddRow(int numColumns, LPARAM lParam);
		void RemoveRow(int rowNum);
		void DeselectAll();
		bool DeleteAllItems();
		void SelectRow(int index);
		void SelectSquareTo(int x, int y);
		void SelectSquare(int xStart, int yStart, int xEnd, int yEnd);
		void FocusItem(int x, int y);

		bool isFocused(int x, int y);
		
		// Checks where a point is in relation to the GridView items
		bool contentHitTest(int x, int y, bool &outsideLeft, bool &outsideTop, bool &outsideRight, bool &outsideBottom);
	
	protected:
		bool resize(int numRows, int numColumns);
	
	private:
		static GridControlItem nullItem; // Used to prevent null reference errors
		GridControlItem** gridItems/*[y][x]*/;
		int numRows;
		int numColumns;
		int focusedX;
		int focusedY;
		bool lButtonDown;

		void DragSelectTo(int x, int y);
		LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif