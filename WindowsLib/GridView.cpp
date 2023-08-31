#include "GridView.h"
#include "../CrossCutLib/Logger.h"
#include <iostream>
#include <string>
#include <SimpleIcu.h>
#include <CommCtrl.h>
#include <WindowsX.h>

extern Logger logger;

namespace WinLib {

    GridControlItem GridViewControl::nullItem; // Obligatory definition of static variable

    GridViewControl::GridViewControl() : gridItems(nullptr), numRows(0), numColumns(0), focusedX(-1), focusedY(-1),
        lButtonDown(false), editing(false), caretPos(0), ending(false)
    {

    }

    GridViewControl::~GridViewControl()
    {
        resize(0, 0);
    }

    GridControlItem & GridViewControl::item(int x, int y)
    {
        if ( y>=0 && y<numRows && x>=0 && x<numColumns )
            return gridItems[y][x];
        else
            return nullItem;
    }

    EditControl & GridViewControl::EditBox()
    {
        return editBox;
    }

    bool GridViewControl::allowKeyNavDuringEdit(GVKey)
    {
        return true;
    }

    bool GridViewControl::allowSel(int, int)
    {
        return true; // Always allow, descendants may alter
    }

    bool GridViewControl::allowSel(int, int, int, int)
    {
        return true; // Always allow, decendants may alter
    }

    void GridViewControl::adjustSel(int &, int &)
    {
        return; // Do nothing, let descendants alter selections
    }

    void GridViewControl::adjustSel(int &, int &, int &, int &)
    {
        return; // Do nothing, let descendants alter selections
    }

    void GridViewControl::CellClicked(int, int)
    {
        return; // Do nothing, let descendants respond if they wish
    }

    bool GridViewControl::CreateThis(HWND hParent, int x, int y, int width, int height, u64 id)
    {
        u32 style = WS_CHILD|WS_CLIPCHILDREN|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_OWNERDRAWFIXED|LVS_OWNERDATA;\

        if ( WindowControl::CreateControl(WS_EX_CLIENTEDGE, WC_LISTVIEW, "", style,
                                          x, y, width, height, hParent, (HMENU)id, false) )
        {
            return RedirectProc();
        }
        return false;
    }

    bool GridViewControl::AddColumn(int insertAt, const std::string & title, int width, int alignmentFlags)
    {
        return resize(numRows, numColumns+1) &&
               ListViewControl::AddColumn(insertAt, title, width, alignmentFlags);
    }

    void GridViewControl::AddRow(int numColumnsInRow, LPARAM lParam)
    {
        if ( resize(numRows+1, std::max(this->numColumns, numColumnsInRow)) )
            ListViewControl::AddRow(numColumnsInRow, lParam);
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
        EndEditing();
        for ( int y=0; y<numRows; y++ )
        {
            for ( int x=0; x<numColumns; x++ )
                gridItems[y][x].SetSelected(false);
        }
    }

    void GridViewControl::ClearItems()
    {
        for ( int y=0; y<numRows; y++ )
        {
            for ( int x=0; x<numColumns; x++ )
                item(x, y).Clear();
        }
        RedrawThis();
    }

    void GridViewControl::DeleteSelection()
    {
        bool hasDeleted = false;
        std::string str = "";
        for ( int y=0; y<numRows; y++ )
        {
            for ( int x=0; x<numColumns; x++ )
            {
                if ( item(x, y).isSelected() || isFocused(x, y) )
                {
                    hasDeleted = true;
                    if ( SendMessage(GetParent(getHandle()), GV::WM_GRIDITEMDELETING, MAKEWPARAM(x, y), 0) == TRUE )
                    {
                        item(x, y).SetText(str);
                        EditTextChanged(str);
                    }
                }
            }
        }

        if ( hasDeleted )
            SendMessage(GetParent(getHandle()), GV::WM_GRIDDELETEFINISHED, 0, 0);
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

        for ( int x = 0; x < numColumns; x++ )
        {
            if ( allowSel(x, index) )
                gridItems[index][x].SetSelected(true);
        }
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
                    for ( int xc = std::min(focusedX, x); xc <= xEnd; xc++ )
                    {
                        if ( allowSel(xc, yc) )
                            gridItems[yc][xc].SetSelected(true);
                    }
                }
            }
            else // Start from 0
            {
                for ( int yc=0; yc<=y; yc++ )
                {
                    for ( int xc = 0; xc <= x; xc++ )
                    {
                        if ( allowSel(xc, yc) )
                            gridItems[yc][xc].SetSelected(true);
                    }
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

            if ( allowSel(xStart, xEnd, yStart, yEnd) )
            {
                adjustSel(xStart, xEnd, yStart, yEnd);
                for ( int yc = yStart; yc <= yEnd; yc++ )
                {
                    for ( int xc = xStart; xc <= xEnd; xc++ )
                    {
                        gridItems[yc][xc].SetSelected(true);
                    }
                }
            }
        }
    }

    void GridViewControl::FocusItem(int x, int y)
    {
        EndEditing();
        if ( x >= 0 && x < numColumns && y >= 0 && y < numRows && allowSel(x, y) )
        {
            if ( x != focusedX || y != focusedY )
            {
                focusedX = x;
                focusedY = y;
                item(x, y).SetSelected(true);
                SendMessage(GetParent(getHandle()), GV::WM_GRIDSELCHANGED, MAKEWPARAM(focusedX, focusedY), 0);
            }
        }
    }

    void GridViewControl::SetEditText(const std::string & text)
    {
        if ( editing )
        {
            editBox.SetText(text);
            SetCaretPos(text.length());
        }
    }

    void GridViewControl::EndEditing()
    {
        if ( !ending )
        {
            ending = true;
            auto str = editBox.GetWinText();
            if ( editing == true && editBox.DestroyThis() )
            {
                editing = false;
                if ( str && SendMessage(GetParent(getHandle()), GV::WM_GRIDITEMCHANGING, MAKEWPARAM(focusedX, focusedY), (LPARAM)&str) == TRUE )
                    item(focusedX, focusedY).SetText(*str);

                RedrawThis();
                SendMessage(GetParent(getHandle()), GV::WM_GRIDEDITEND, MAKEWPARAM(focusedX, focusedY), 0);
            }
            ending = false;
        }
    }

    void GridViewControl::AutoSizeColumns(int minWidth, int maxWidth)
    {
        for ( int i=0; i<numColumns; i++ )
            AutoSizeColumn(i, minWidth, maxWidth);
    }

    int GridViewControl::NumRows()
    {
        return numRows;
    }

    int GridViewControl::NumColumns()
    {
        return numColumns;
    }

    bool GridViewControl::isEditing()
    {
        return editing;
    }

    bool GridViewControl::isEditing(int x, int y)
    {
        return editing && x == focusedX && y == focusedY;
    }

    bool GridViewControl::isFocused(int x, int y)
    {
        return x == focusedX && y == focusedY;
    }

    bool GridViewControl::isSelectionRectangular()
    {
        int topLeftX = -1, topLeftY = -1,
            bottomRightX = -1, bottomRightY = -1;

        for ( int y = 0; y < numRows; y++ )
        {
            for ( int x = 0; x < numColumns; x++ )
            {
                if ( isFocused(x, y) || item(x, y).isSelected() )
                {
                    topLeftX = x;
                    topLeftY = y;
                    goto ExitTopLeftLoop; // Found a top-left coordinate
                }
            }
        } ExitTopLeftLoop:

        if ( topLeftX == -1 || topLeftY == -1 )
            return false; // No selection found

        for ( int y = numRows-1; y >= 0; y-- )
        {
            for ( int x = numColumns-1; x >= 0; x-- )
            {
                if ( isFocused(x, y) || item(x, y).isSelected() )
                {
                    bottomRightX = x;
                    bottomRightY = y;
                    goto ExitBottomRightLoop; // Found a bottom-right coordinate
                }
            }
        } ExitBottomRightLoop:

        if ( bottomRightX < topLeftX ) // Selection forms a backwards L
            return false; // Backwards L's cannot be rectangular

        if ( topLeftX != -1 && topLeftY != -1 && bottomRightX != -1 && bottomRightY != -1 ) // Found coordinates
        {
            for ( int y = 0; y < numRows; y++ )
            {
                for ( int x = 0; x < numColumns; x++ )
                {
                    if ( isFocused(x, y) || item(x, y).isSelected() )
                    {
                        if ( x < topLeftX || x > bottomRightX || y < topLeftY || y > bottomRightY )
                            return false; // Coordinate of selected item is outside rectangle formed by topLeft/bottomRight coordinates
                    }
                }
            }

            for ( int y = topLeftY; y < bottomRightY; y++ )
            {
                for ( int x = topLeftX; x < bottomRightX; x++ )
                {
                    if ( !(isFocused(x, y) || item(x, y).isSelected()) )
                        return false; // Coordinate within rectangle formed by topLeft/bottomRight is not selected
                }
            }
            return true;
        }
        return false;
    }

    bool GridViewControl::GetSelTopLeft(int & topLeftX, int & topLeftY)
    {
        topLeftX = -1;
        topLeftY = -1;
        for ( int y = 0; y < numRows; y++ )
        {
            for ( int x = 0; x < numColumns; x++ )
            {
                if ( isFocused(x, y) || item(x, y).isSelected() )
                {
                    topLeftX = x;
                    topLeftY = y;
                    return true;
                }
            }
        }
        return false;
    }

    bool GridViewControl::GetFocusedItem(int & x, int & y)
    {
        x = focusedX;
        y = focusedY;
        return focusedX != -1 && focusedY != -1;
    }

    bool GridViewControl::GetFocusedItemRect(RECT & rect)
    {
        return ListViewControl::GetItemRect(focusedX, focusedY, rect);
    }

    POINT GridViewControl::GetItemTopLeft(int x, int y)
    {
        POINT pt = {};
        RECT rcItem = {};
        if ( GetItemRect(x, y, rcItem) )
        {
            pt.x = rcItem.left;
            pt.y = rcItem.top;
        }
        return pt;
    }

    POINT GridViewControl::GetFocusedBottomRightScreenPt()
    {
        POINT pt = {};
        RECT rcFocus = {};
        if ( GetFocusedItemRect(rcFocus) )
        {
            pt.x = rcFocus.right;
            pt.y = rcFocus.bottom;
            if ( ClientToScreen(getHandle(), &pt) )
                return pt;
        }
        pt.x = -1;
        pt.y = -1;
        return pt;
    }

    bool GridViewControl::GetEditItemRect(RECT & rect)
    {
        return editing && ListViewControl::GetItemRect(focusedX, focusedY, rect);
    }
    
    std::optional<std::string> GridViewControl::GetEditText()
    {
        return editBox.GetWinText();
    }

    bool GridViewControl::contentHitTest(int x, int y, bool & outsideLeft, bool & outsideTop, bool & outsideRight, bool & outsideBottom)
    {
        if ( numRows > 0 && numColumns > 0 )
        {
            RECT topBounds = { };
            RECT bottomBounds = { };
            topBounds.left = LVIR_BOUNDS;
            bottomBounds.left = LVIR_BOUNDS;
            if ( SendMessage(getHandle(), LVM_GETITEMRECT, 0, (LPARAM)&topBounds) == TRUE &&
                 SendMessage(getHandle(), LVM_GETITEMRECT, WPARAM(numRows)-1, (LPARAM)&bottomBounds) == TRUE )
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

    bool GridViewControl::resize(int newNumRows, int newNumColumns)
    {
        if ( newNumRows > 0 && newNumColumns > 0 )
        {
            GridControlItem** newGridItems;
            try {
                newGridItems = new GridControlItem*[newNumRows];
                for ( int y=0; y<newNumRows; y++ )
                    newGridItems[y] = new GridControlItem[newNumColumns];
            }
            catch ( ... ) { return false; }

            int maxCopyRow = std::min(newNumRows, this->numRows);
            int maxCopyColumn = std::min(newNumColumns, this->numColumns);
            for ( int y=0; y<maxCopyRow; y++ )
            {
                for ( int x=0; x<maxCopyColumn; x++ )
                    newGridItems[y][x] = gridItems[y][x];
            }

            for ( int y=0; y<this->numRows; y++ )
                delete[] gridItems[y];
            delete[] gridItems;

            gridItems = newGridItems;
            this->numRows = newNumRows;
            this->numColumns = newNumColumns;
            return true;
        }
        else if ( newNumRows == 0 || newNumColumns == 0 )
        {

            if ( this->numRows > 0 && this->numColumns > 0 )
            {
                for ( int y=0; y<this->numRows; y++ )
                    delete[] gridItems[y];
                delete[] gridItems;
            }
            gridItems = nullptr;
            this->numRows = newNumRows;
            this->numColumns = newNumColumns;
            return true;
        }
        return false;
    }

    void GridViewControl::AutoSizeColumn(int x, int minWidth, int maxWidth)
    {
        int newWidth = 0;
        for ( int y=0; y<numRows; y++ )
        {
            RECT rect;
            std::string str;
            if ( item(x, y).getText(str) &&
                 ListViewControl::GetItemRect(focusedX, focusedY, rect) )
            {
                int itemWidth = (int)SendMessage(GetParent(getHandle()), GV::WM_GETGRIDITEMWIDTH, MAKEWPARAM(x, y), 0);
                if ( itemWidth > newWidth )
                    newWidth = itemWidth;
            }
        }

        if ( newWidth > 0 )
        {
            if ( minWidth > 0 && newWidth < minWidth )
                newWidth = minWidth;

            if ( newWidth > maxWidth )
                newWidth = maxWidth;

            if ( minWidth == 0 && newWidth == ListViewControl::GetColumnWidth(x) )
            {
                MEASUREITEMSTRUCT mis = { };
                mis.CtlType = ODT_LISTVIEW;
                mis.CtlID = GetDlgCtrlID(getHandle());
                mis.itemID = 0;
                mis.itemWidth = 0;
                mis.itemHeight = 0;
                mis.itemData = 0;
                if ( SendMessage(GetParent(getHandle()), WM_MEASUREITEM, (WPARAM)mis.CtlID, (LPARAM)&mis) == TRUE )
                    ListViewControl::SetColumnWidth(x, mis.itemWidth);
            }
            else
                ListViewControl::SetColumnWidth(x, newWidth);
        }
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

    void GridViewControl::StartEditing(int xClick, int yClick, const std::string & initChar)
    {
        RECT rect = { };
        if ( editing == false &&
             GetItemRect(focusedX, focusedY, rect) &&
             editBox.CreateThis(getHandle(), rect.left+1, rect.top, rect.right-rect.left-2, rect.bottom-rect.top-1, false, false, 0) )
        {
            editing = true;

            editBox.setDefaultFont(false);
            bool startedByInput = false;

            int x = -1;
            int y = -1;
            if ( initChar.empty() && GetItemAt(xClick, yClick, y, x) )
            {
                std::string str;
                if ( item(x, y).getText(str) )
                    editBox.SetText(str);
            
                if ( xClick >= 0 && yClick >= 0 )
                    UpdateCaretPos(xClick, yClick);
            }
            else if ( initChar == "\b" )
            {
                editBox.SetText("");
                SetCaretPos(0);
            }
            else if ( !initChar.empty() ) // By input start
            {
                startedByInput = true;
                editBox.SetForwardArrowKeys(true);
                editBox.SetText(initChar);
                SetCaretPos(1);
            }
            editBox.ExpandToText();
            RedrawThis();

            SendMessage(GetParent(getHandle()), GV::WM_GRIDEDITSTART, MAKEWPARAM(focusedX, focusedY), 0);

            if ( startedByInput )
                EditTextChanged(initChar);
        }
    }

    void GridViewControl::UpdateCaretPos(int xClick, int yClick)
    {
        int x = -1;
        int y = -1;
        RECT rect = { };
        if ( GetItemAt(xClick, yClick, y, x) &&
             GetItemRect(x, y, rect) &&
             xClick >= rect.left && xClick <= rect.right &&
             yClick >= rect.top && yClick <= rect.bottom )
        {
            int cx = xClick - rect.left;
            int cy = yClick - rect.top;
            /** Caret position is based on how the parent draws items,
                so request the caret position from the parent based on
                the position of the click within the given item
            
                In the future the GridView could decide the caret pos
                if their was an option for 'default drawing' within a
                GridView (and use this if the request failed), though
                there is not currently, so in this case use the last
                position (text length). */
            LRESULT result = SendMessage(GetParent(getHandle()), GV::WM_GETGRIDITEMCARETPOS, MAKEWPARAM(x, y), MAKELPARAM(cx, cy));
            if ( result != -1 )
                SetCaretPos((int)result);
            else
            {
                result = SendMessage(editBox.getHandle(), EM_CHARFROMPOS, 0, MAKELPARAM(cx, cy));
                SetCaretPos(int(LOWORD(result)));
            }
        }
        else
            SetCaretPos(item(x, y).getTextLength());
    }

    void GridViewControl::SetCaretPos(size_t newCaretPos)
    {
        caretPos = newCaretPos;
        SendMessage(editBox.getHandle(), EM_SETSEL, (WPARAM)caretPos, (LPARAM)caretPos);
        editBox.FocusThis();
    }

    void GridViewControl::Char(const std::string & character)
    {
        if ( !(GetKeyState(VK_CONTROL) & 0x8000) )
            StartEditing(-1, -1, character);
    }

    void GridViewControl::KeyDown(WPARAM wParam)
    {
        switch ( wParam )
        {
            case VK_LEFT: LeftArrowKey(); break;
            case VK_UP: UpArrowKey(); break;
            case VK_RIGHT: RightArrowKey(); break;
            case VK_DOWN: DownArrowKey(); break;
            case VK_TAB: TabKey(); break;
            case VK_RETURN: EnterKey(); break;
            case VK_DELETE: DeleteSelection(); break;
            default: return; break;
        }

        RedrawThis();
    }

    void GridViewControl::EditTextChanged(const std::string &)
    {

    }

    void GridViewControl::LButtonDown(int xClick, int yClick)
    {
        bool allowDrag = true;

        FocusThis();
        int x = -1;
        int y = -1;

        if ( ListViewControl::GetItemAt(xClick, yClick, y, x) )
        {
            CellClicked(x, y);
            if ( allowSel(x, y) )
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
                        FocusItem(x, y);
                }
                else // Normal click within items
                {
                    DeselectAll();
                    //if ( editing && focusedX == x && focusedY == y ) // Use if manually doing edits
                    //  UpdateCaretPos(xClick, yClick);
                    FocusItem(x, y);
                }
                RedrawThis();
            }
            else // Selection at (x, y) is forbidden
                allowDrag = false;
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

        if ( allowDrag )
        {
            SetCapture(getHandle());
            lButtonDown = true;
            TrackMouse(10);
        }
    }

    void GridViewControl::LButtonDblClk(int xClick, int yClick)
    {
        int x = -1;
        int y = -1;
        if ( ListViewControl::GetItemAt(xClick, yClick, y, x) && x == focusedX && y == focusedY )
            StartEditing(xClick, yClick, "");
    }

    void GridViewControl::MouseMove(int xMove, int yMove, WPARAM wParam)
    {
        if ( lButtonDown && focusedX != -1 && focusedY != -1 && wParam & MK_LBUTTON ) // If click and dragging
        {
            int dragMoveX = -1;
            int dragMoveY = -1;
            if ( !editing && ListViewControl::GetItemAt(xMove, yMove, dragMoveY, dragMoveX) )
            {
                if ( !(GetKeyState(VK_CONTROL) & 0x8000) )
                    DeselectAll();

                SelectSquare(focusedX, focusedY, dragMoveX, dragMoveY);
                RedrawThis();
            }
        }
    }

    void GridViewControl::MouseHover(int xHover, int yHover)
    {
        if ( !editing && lButtonDown && focusedX != -1 && focusedY != -1 )
        {
            DragSelectTo(xHover, yHover);
            TrackMouse(10);
        }
    }

    void GridViewControl::LButtonUp(int xRelease, int yRelease)
    {
        if ( lButtonDown && !editing && focusedX != -1 && focusedY != -1 )
        {
            lButtonDown = false;
            ReleaseCapture();
            DragSelectTo(xRelease, yRelease);
        }
    }

    void GridViewControl::LeftArrowKey()
    {
        // Focus next selectable cell directly to the left of focus
        if ( !editing || allowKeyNavDuringEdit(GVKey::GVLeft) )
        {
            DeselectAll();

            for ( int x = focusedX - 1; x >= 0; x-- )
            {
                if ( allowSel(x, focusedY) )
                {
                    FocusItem(x, focusedY);
                    return;
                }
            }
        }
    }

    void GridViewControl::UpArrowKey()
    {
        // Focus next selectable cell directly above focus
        if ( !editing || allowKeyNavDuringEdit(GVKey::GVUp) )
        {
            DeselectAll();

            for ( int y = focusedY - 1; y >= 0; y-- )
            {
                if ( allowSel(focusedX, y) )
                {
                    FocusItem(focusedX, y);
                    return;
                }
            }
        }
    }

    void GridViewControl::RightArrowKey()
    {
        // Focus next selectable cell directly to the right of focus
        if ( !editing || allowKeyNavDuringEdit(GVKey::GVRight) )
        {
            DeselectAll();

            for ( int x = focusedX + 1; x < numColumns; x++ )
            {
                if ( allowSel(x, focusedY) )
                {
                    FocusItem(x, focusedY);
                    return;
                }
            }
        }
    }

    void GridViewControl::DownArrowKey()
    {
        // Focus next selectable cell directly below focus
        if ( !editing || allowKeyNavDuringEdit(GVKey::GVDown) )
        {
            DeselectAll();

            for ( int y = focusedY + 1; y < numRows; y++ )
            {
                if ( allowSel(focusedX, y) )
                {
                    FocusItem(focusedX, y);
                    return;
                }
            }
        }
    }

    void GridViewControl::TabKey()
    {
        // Focus next selectable cell
        if ( !editing || allowKeyNavDuringEdit(GVKey::GVTab) )
        {
            DeselectAll();

            for ( int x = focusedX + 1; x < numColumns; x++ )
            {
                if ( allowSel(x, focusedY) )
                {
                    FocusItem(x, focusedY);
                    return;
                }
            }

            for ( int y = focusedY + 1; y < numRows; y++ )
            {
                for ( int x = 0; x < numColumns; x++ )
                {
                    if ( allowSel(x, y) )
                    {
                        FocusItem(x, y);
                        return;
                    }
                }
            }
        }
    }

    void GridViewControl::EnterKey()
    {
        // Focus first selectable cell lower rows
        if ( !editing || allowKeyNavDuringEdit(GVKey::GVReturn) )
        {
            DeselectAll();

            for ( int y = focusedY + 1; y < numRows; y++ )
            {
                for ( int x = 0; x < numColumns; x++ )
                {
                    if ( allowSel(x, y) )
                    {
                        FocusItem(x, y);
                        return;
                    }
                }
            }
        }
    }

    void GridViewControl::DrawItems(HWND hWnd)
    {
        HWND hWndParent = GetParent(hWnd);
        UINT ctrlId = (UINT)GetDlgCtrlID(hWnd);
        PAINTSTRUCT ps {};
        DRAWITEMSTRUCT dis {};
        dis.CtlType = ODT_LISTVIEW;
        dis.CtlID = ctrlId;
        dis.itemID = 0; // Row number
        dis.itemAction = ODA_DRAWENTIRE;
        dis.itemState = 0; // Specific to the item
        dis.hwndItem = hWnd;
        dis.hDC = NULL; // Set this once a context has been allocated
        dis.rcItem.left = 0; // Specific to the item
        dis.rcItem.top = 0; // Specific to the item
        dis.rcItem.right = 0; // Specific to the item
        dis.rcItem.bottom = 0; // Specific to the item
        dis.itemData = 0; // This is ununsed since data is specific to the subitems
        LPARAM rectPointer = LPARAM(&dis.rcItem);
        LPARAM disPointer = LPARAM(&dis);

        RECT rcCli {};
        if ( getClientRect(rcCli) )
        {
            WinLib::DeviceContext dc(getHandle(), rcCli.right-rcCli.left, rcCli.bottom-rcCli.top);
            dc.fillRect(rcCli, dc.getBkColor());

            dis.hDC = dc.getDcHandle();
            for ( int y=0; y<numRows; y++ )
            {
                dis.itemID = (UINT)y;
                LPARAM state = SendMessage(hWnd, LVM_GETITEMSTATE, (WPARAM)y, LVIS_FOCUSED|LVIS_SELECTED);
                if ( state & LVIS_FOCUSED ) dis.itemState |= ODS_FOCUS;
                if ( state & LVIS_SELECTED ) dis.itemState |= ODS_SELECTED;
                dis.rcItem.left = LVIR_BOUNDS;
                if ( SendMessage(hWnd, LVM_GETITEMRECT, (WPARAM)y, rectPointer) == TRUE )
                    SendMessage(hWndParent, GV::WM_DRAWGRIDVIEWITEM, ctrlId, disPointer);
            }

            SendMessage(hWndParent, GV::WM_DRAWTOUCHUPS, (WPARAM)dis.hDC, 0);
            dc.flushBuffer();
        }
    }

    void GridViewControl::Paint(HWND hWnd)
    {
        SendMessage(GetParent(hWnd), LB::WM_PREDRAWITEMS, 0, (LPARAM)hWnd);
        DrawItems(hWnd);
        SendMessage(GetParent(hWnd), LB::WM_POSTDRAWITEMS, 0, (LPARAM)hWnd);
    }

    LRESULT GridViewControl::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
    {
        switch ( nmhdr->code )
        {
            case HDN_DIVIDERDBLCLICKA: AutoSizeColumn(((NMHEADERA*)nmhdr)->iItem, 0, 100); break;
            case HDN_DIVIDERDBLCLICKW: AutoSizeColumn(((NMHEADERW*)nmhdr)->iItem, 0, 100); break;
            default: return WindowControl::Notify(hWnd, idFrom, nmhdr); break;
        }
        return 0;
    }

    LRESULT GridViewControl::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
    {
        return WindowControl::Command(hWnd, wParam, lParam);
    }

    LRESULT GridViewControl::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch ( msg )
        {
            case WM_SETFOCUS: EndEditing(); break; // The GridView rather than the edit box was focused
            case WM_KEYDOWN: KeyDown(wParam); break;
            case WM_CHAR: Char(icux::toUtf8((wchar_t)wParam)); break;
            case WM_UNICHAR: Char(icux::toUtf8((char32_t)wParam)); break;
            case WM_LBUTTONDOWN: LButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
            case WM_MOUSEMOVE: MouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam); break;
            case WM_MOUSEHOVER: MouseHover(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
            case WM_LBUTTONUP: LButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
            case WM_LBUTTONDBLCLK: LButtonDblClk(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
            case WM_PAINT: Paint(hWnd); break;
            default: return WindowControl::ControlProc(hWnd, msg, wParam, lParam); break;
        }
        return 0;
    }

}
