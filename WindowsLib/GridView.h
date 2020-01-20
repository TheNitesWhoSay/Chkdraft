#ifndef GRIDVIEW_H
#define GRIDVIEW_H
#include "ListViewControl.h"
#include "GridItem.h"
#include "EditControl.h"
#include <string>

namespace WinLib {

    /** A modified ListView for working with items as
        if they were individual units within an x/y grid
        may use some or all of the GV:: messages from
        Enumerations.h */

    enum class GVKey
    {
        GVLeft,
        GVUp,
        GVRight,
        GVDown,
        GVReturn,
        GVTab
    };

    class GridViewControl : public ListViewControl
    {
        public:
            GridViewControl();
            virtual ~GridViewControl();
            GridControlItem & item(int x, int y);
            EditControl & EditBox();

            virtual bool allowKeyNavDuringEdit(GVKey key); // Checks if navigation is allowed by the specified key
            virtual bool allowSel(int x, int y);
            virtual bool allowSel(int xStart, int xEnd, int yStart, int yEnd);
            virtual void adjustSel(int & x, int & y);
            virtual void adjustSel(int & xStart, int & xEnd, int & yStart, int & yEnd);
            virtual void CellClicked(int x, int y);

            bool CreateThis(HWND hParent, int x, int y, int width, int height, u64 id);
            bool AddColumn(int insertAt, const std::string & title, int width, int alignmentFlags);
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
            void SetEditText(const std::string & text);
            void EndEditing();
            void AutoSizeColumns(int minWidth, int maxWidth);
        
            int NumRows();
            int NumColumns();
            bool isEditing();
            bool isEditing(int x, int y);
            bool isFocused(int x, int y);
            bool isSelectionRectangular();
            bool GetSelTopLeft(int & topLeftX, int & topLeftY);
            bool GetFocusedItem(int & x, int & y);
            bool GetFocusedItemRect(RECT & rect);
            POINT GetItemTopLeft(int x, int y);
            POINT GetFocusedBottomRightScreenPt();
            bool GetEditItemRect(RECT & rect);
            bool GetEditText(output_param std::string & str);
        
            // Checks where a point is in relation to the GridView items
            bool contentHitTest(int x, int y, bool & outsideLeft, bool & outsideTop, bool & outsideRight, bool & outsideBottom);
    
        protected:
            bool resize(int numRows, int numColumns);
            void AutoSizeColumn(int x, int minWidth, int maxWidth);
            void DragSelectTo(int x, int y);
            virtual void StartEditing(int xClick, int yClick, const std::string & initChar); // Pass -1, -1 if starting via typing, "" if by click
            void UpdateCaretPos(int xClick, int yClick);
            void SetCaretPos(size_t newCaretPos);
            virtual void Char(const std::string & character);
            virtual void KeyDown(WPARAM wParam);

            /** Called when the GridView alters text in the edit box manually,
                does not get called when the edit box text is changed via
                regular user input to the edit control */
            virtual void EditTextChanged(const std::string & str);

            virtual void Paint(HWND hWnd);
            virtual LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
            virtual LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
            virtual LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
        private:
            static GridControlItem nullItem; // Used to prevent null reference errors
            GridControlItem** gridItems/*[y][x]*/;
            int numRows;
            int numColumns;
            int focusedX;
            int focusedY;
            bool lButtonDown;
            bool editing;
            size_t caretPos;
            EditControl editBox;
            bool ending;

            void LButtonDown(int xClick, int yClick);
            void LButtonDblClk(int xClick, int yClick);
            void MouseMove(int xMove, int yMove, WPARAM wParam);
            void MouseHover(int xHover, int yHover);
            void LButtonUp(int xRelease, int yRelease);
            void LeftArrowKey();
            void UpArrowKey();
            void RightArrowKey();
            void DownArrowKey();
            void TabKey();
            void EnterKey();
            void DrawItems(HWND hWnd);
        
        //protected:
            //virtual LRESULT Notify(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
            //virtual LRESULT Command(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
            //virtual LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };

}

#endif