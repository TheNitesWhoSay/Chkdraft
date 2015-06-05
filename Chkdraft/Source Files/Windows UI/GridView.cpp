#include "GridView.h"
#include <iostream>
using namespace std;

GridControlItem GridViewControl::nullItem; // Obligatory definition of static variable

GridViewControl::GridViewControl() : gridItems(nullptr), numRows(0), numColumns(0), focusedX(-1), focusedY(-1), lButtonDown(false)
{

}

GridControlItem& GridViewControl::item(int x, int y)
{
	if ( y>=0 && y<=numRows && x>=0 && x<=numColumns )
		return gridItems[y][x];
	else
		return nullItem;
}

bool GridViewControl::CreateThis(HWND hParent, int x, int y, int width, int height, bool editable, u32 id)
{
	u32 style = WS_CHILD|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_OWNERDRAWFIXED|LVS_OWNERDATA;
	//if ( editable )
	//	style |= LVS_EDITLABELS;

	if ( WindowControl::CreateControl(WS_EX_CLIENTEDGE, WC_LISTVIEW, "", style,
									  x, y, width, height, hParent, (HMENU)id, false) )
	{
		return RedirectProc();
	}
	return false;
}

void GridViewControl::AddColumn(int insertAt, const char* title, int width, int alignmentFlags)
{
	if ( resize(numRows, numColumns+1) )
		ListViewControl::AddColumn(insertAt, title, width, alignmentFlags);
}

void GridViewControl::AddRow(int numColumns, LPARAM lParam)
{
	if ( resize(numRows+1, this->numColumns) )
		ListViewControl::AddRow(numColumns, lParam);
}

void GridViewControl::RemoveRow(int rowNum)
{
	for ( int y=rowNum; y<numRows-1; y++ )
	{
		for ( int x=0; x<numColumns; x++ )
			gridItems[y][x] = gridItems[y+1][x];
	}

	if ( resize(numRows-1, numColumns) )
		ListViewControl::RemoveRow(rowNum);
}

void GridViewControl::DeselectAll()
{
	for ( int y=0; y<numRows; y++ )
	{
		for ( int x=0; x<numColumns; x++ )
			gridItems[y][x].SetSelected(false);
	}
}

bool GridViewControl::DeleteAllItems()
{
	return ListViewControl::DeleteAllItems() &&
		   resize(0, numColumns);
}

void GridViewControl::SelectRow(int index)
{
	for ( int y=0; y<numRows; y++ )
	{
		for ( int x=0; x<numColumns; x++ )
			gridItems[y][x].SetSelected(false);
	}

	for ( int x=0; x<numColumns; x++ )
		gridItems[index][x].SetSelected(true);
}

void GridViewControl::SelectSquareTo(int x, int y)
{
	if ( x >= 0 && x < numColumns && y >= 0 && y < numRows )
	{
		if ( focusedX >= 0 && focusedX < numColumns && focusedY >= 0 && focusedY < numRows ) // Start from focus
		{
			int xEnd = std::max(focusedX, x);
			int yEnd = std::max(focusedY, y);
			for ( int yc=std::min(focusedY, y); yc<=yEnd; yc++ )
			{
				for ( int xc=std::min(focusedX, x); xc<=xEnd; xc++ )
					gridItems[yc][xc].SetSelected(true);
			}
		}
		else // Start from 0
		{
			for ( int yc=0; yc<=y; yc++ )
			{
				for ( int xc=0; xc<=x; xc++ )
					gridItems[yc][xc].SetSelected(true);
			}
		}
	}
}

void GridViewControl::SelectSquare(int xStart, int yStart, int xEnd, int yEnd)
{
	if ( xStart >= 0 && xStart < numColumns && yStart >= 0 && yStart < numRows &&
		 xEnd >= 0 && xEnd < numColumns && yEnd >= 0 && yEnd < numRows )
	{
		if ( xStart > xEnd )
			std::swap(xStart, xEnd);

		if ( yStart > yEnd )
			std::swap(yStart, yEnd);

		for ( int yc=yStart; yc<=yEnd; yc++ )
		{
			for ( int xc=xStart; xc<=xEnd; xc++ )
				gridItems[yc][xc].SetSelected(true);
		}
	}
}

void GridViewControl::FocusItem(int x, int y)
{
	if ( x >= 0 && x < numColumns && y >= 0 && y < numRows )
	{
		focusedX = x;
		focusedY = y;
	}
	else
	{
		focusedX = -1;
		focusedY = -1;
	}
}

bool GridViewControl::isFocused(int x, int y)
{
	return x == focusedX && y == focusedY;
}

bool GridViewControl::contentHitTest(int x, int y, bool &outsideLeft, bool &outsideTop, bool &outsideRight, bool &outsideBottom)
{
	if ( numRows > 0 && numColumns > 0 )
	{
		RECT topBounds = { };
		RECT bottomBounds = { };
		topBounds.left = LVIR_BOUNDS;
		bottomBounds.left = LVIR_BOUNDS;
		if ( SendMessage(getHandle(), LVM_GETITEMRECT, 0, (LPARAM)&topBounds) == TRUE &&
			 SendMessage(getHandle(), LVM_GETITEMRECT, numRows-1, (LPARAM)&bottomBounds) == TRUE )
		{
			outsideLeft = x < topBounds.left;
			outsideTop = y < topBounds.top;
			outsideRight = x > bottomBounds.right;
			outsideBottom = y > bottomBounds.bottom;
			return true;
		}
	}
	return false;
}

bool GridViewControl::resize(int numRows, int numColumns)
{
	if ( numRows > 0 && numColumns > 0 )
	{
		GridControlItem** newGridItems;
		try {
			newGridItems = new GridControlItem*[numRows];
			for ( int y=0; y<numRows; y++ )
				newGridItems[y] = new GridControlItem[numColumns];
		}
		catch ( std::bad_alloc ) { return false; }

		int maxCopyRow = std::min(numRows, this->numRows);
		int maxCopyColumn = std::min(numColumns, this->numColumns);
		for ( int y=0; y<maxCopyRow; y++ )
		{
			for ( int x=0; x<maxCopyColumn; x++ )
				newGridItems[y][x] = gridItems[y][x];
		}

		for ( int y=0; y<this->numRows; y++ )
			delete[] gridItems[y];
		delete[] gridItems;

		gridItems = newGridItems;
		this->numRows = numRows;
		this->numColumns = numColumns;
		return true;
	}
	else if ( numRows == 0 || numColumns == 0 )
	{

		if ( this->numRows > 0 && this->numColumns > 0 )
		{
			for ( int y=0; y<this->numRows; y++ )
				delete[] gridItems[y];
			delete[] gridItems;
		}
		gridItems = nullptr;
		this->numRows = numRows;
		this->numColumns = numColumns;
		return true;
	}
	return false;
}

void GridViewControl::DragSelectTo(int x, int y)
{
	int dragMoveX = -1;
	int dragMoveY = -1;

	bool outsideLeft = false, outsideTop = false, outsideRight = false, outsideBottom = false;
	if ( contentHitTest(x, y, outsideLeft, outsideTop, outsideRight, outsideBottom) &&
		 ( outsideLeft || outsideTop || outsideRight || outsideBottom ) )
	{
		if ( !(GetKeyState(VK_CONTROL) & 0x8000) )
		DeselectAll();

		int row, col;
		if ( outsideLeft && outsideTop )
			SelectSquare(focusedX, focusedY, 0, 0);
		else if ( outsideLeft && outsideBottom )
			SelectSquare(focusedX, focusedY, 0, numRows-1);
		else if ( outsideRight && outsideTop )
			SelectSquare(focusedX, focusedY, numColumns-1, 0);
		else if ( outsideRight && outsideBottom )
			SelectSquare(focusedX, focusedY, numColumns-1, numRows-1);
		else if ( outsideLeft )
		{
			if ( ListViewControl::GetRowAt(y, row) )
				SelectSquare(focusedX, focusedY, 0, row);
		}
		else if ( outsideTop )
		{
			if ( ListViewControl::GetColumnAt(x, col) )
				SelectSquare(focusedX, focusedY, col, 0);
		}
		else if ( outsideRight )
		{
			if ( ListViewControl::GetRowAt(y, row) )
				SelectSquare(focusedX, focusedY, numColumns-1, row);
		}
		else if ( outsideBottom )
		{
			if ( ListViewControl::GetColumnAt(x, col) )
				SelectSquare(focusedX, focusedY, col, numRows-1);
		}
		RedrawThis();
	}
	else if ( ListViewControl::GetItemAt(x, y, dragMoveY, dragMoveX) )
	{
		if ( !(GetKeyState(VK_CONTROL) & 0x8000) )
			DeselectAll();
		
		SelectSquare(focusedX, focusedY, dragMoveX, dragMoveY);
		RedrawThis();
	}
}

LRESULT GridViewControl::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_LBUTTONDOWN:
			{
				int xClick = GET_X_LPARAM(lParam);
				int yClick = GET_Y_LPARAM(lParam);
				int x = -1;
				int y = -1;

				if ( ListViewControl::GetItemAt(xClick, yClick, y, x) )
				{
					if ( (GetKeyState(VK_SHIFT) & 0x8000) && (GetKeyState(VK_CONTROL) & 0x8000) )
					{
						SelectSquareTo(x, y);
					}
					else if ( GetKeyState(VK_SHIFT) & 0x8000 )
					{
						DeselectAll();
						SelectSquareTo(x, y);
					}
					else if ( GetKeyState(VK_CONTROL) & 0x8000 )
					{
						if ( item(x, y).isSelected() )
						{
							if ( !isFocused(x, y) )
								item(x, y).SetSelected(false);
						}
						else
						{
							item(x, y).SetSelected(true);
							FocusItem(x, y);
						}
					}
					else
					{
						DeselectAll();
						item(x, y).SetSelected(true);
						FocusItem(x, y);
					}
					RedrawThis();
				}
				else
				{
					focusedX = -1;
					focusedY = -1;
					DeselectAll();
					bool outsideLeft = false, outsideTop = false, outsideRight = false, outsideBottom = false;
					if ( contentHitTest(xClick, yClick, outsideLeft, outsideTop, outsideRight, outsideBottom) &&
						 ( outsideLeft || outsideTop || outsideRight || outsideBottom ) )
					{
						int row, col;
						if ( outsideLeft && outsideTop )
							FocusItem(0, 0);
						else if ( outsideLeft && outsideBottom )
							FocusItem(0, numRows-1);
						else if ( outsideRight && outsideTop )
							FocusItem(numColumns-1, 0);
						else if ( outsideRight && outsideBottom )
							FocusItem(numColumns-1, numRows-1);
						else if ( outsideLeft )
						{
							if ( ListViewControl::GetRowAt(yClick, row) )
								FocusItem(0, row);
						}
						else if ( outsideTop )
						{
							if ( ListViewControl::GetColumnAt(xClick, col) )
								FocusItem(col, 0);
						}
						else if ( outsideRight )
						{
							if ( ListViewControl::GetRowAt(yClick, row) )
								FocusItem(numColumns-1, row);
						}
						else if ( outsideBottom )
						{
							if ( ListViewControl::GetColumnAt(xClick, col) )
								FocusItem(col, numRows-1);
						}
					}
					RedrawThis();
				}

				SetCapture(getHandle());
				lButtonDown = true;
				TrackMouse(10);
				return 0;
			}
			break;
		case WM_MOUSEMOVE:
			if ( focusedX != -1 && focusedY != -1 && wParam & MK_LBUTTON ) // If click and dragging
			{
				int xRelease = GET_X_LPARAM(lParam);
				int yRelease = GET_Y_LPARAM(lParam);
				int dragMoveX = -1;
				int dragMoveY = -1;
				if ( ListViewControl::GetItemAt(xRelease, yRelease, dragMoveY, dragMoveX) )
				{
					if ( !(GetKeyState(VK_CONTROL) & 0x8000) )
						DeselectAll();
					
					SelectSquare(focusedX, focusedY, dragMoveX, dragMoveY);
					RedrawThis();
				}
			}
			break;
		case WM_MOUSEHOVER:
			if ( lButtonDown && focusedX != -1 && focusedY != -1 )
			{
				int xHover = GET_X_LPARAM(lParam);
				int yHover = GET_Y_LPARAM(lParam);
				DragSelectTo(xHover, yHover);
				TrackMouse(10);
			}
			break;
		case WM_LBUTTONUP:
			if ( focusedX != -1 && focusedY != -1 )
			{
				lButtonDown = false;
				ReleaseCapture();
				int xRelease = GET_X_LPARAM(lParam);
				int yRelease = GET_Y_LPARAM(lParam);
				DragSelectTo(xRelease, yRelease);
			}
			break;
		case WM_KEYDOWN:
			{

			}
			break;
	}
	return WindowControl::CallDefaultProc(hWnd, msg, wParam, lParam); // Take default action
}
