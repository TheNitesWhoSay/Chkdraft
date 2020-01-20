#ifndef LISTVIEWCONTROL_H
#define LISTVIEWCONTROL_H
#include "WindowControl.h"

namespace WinLib {

    class ListViewControl : public WindowControl
    {
        public:
            virtual ~ListViewControl();
            bool CreateThis(HWND hParent, int x, int y, int width, int height, bool editable, bool ownerDrawn, u64 id);
            bool AddColumn(int insertAt, const std::string & title, int width, int alignmentFlags);
            bool SetColumnWidth(int column, int width);
            void AddRow(int numColumns, LPARAM lParam);
            void AddRow();
            void RemoveRow(int rowNum);
            void SetItemText(int row, int column, const std::string & title);
            void SetItemText(int row, int column, int value);
            void SetItemData(int row, int column, u32 data);
            void EnableFullRowSelect();
            bool EnsureVisible(int index, bool ensureEntirelyVisible);
            bool DeleteAllItems();
            bool FocusItem(int index);
            bool SelectRow(int index);
            void RedrawHeader();
            //bool SortItems(WPARAM value, LPARAM function);
        
            int GetItemRow(int lParam);
            int GetNumColumns();
            int GetColumnWidth(int column);
            bool GetItemAt(int xCoord, int yCoord, int & itemRow, int & itemColumn);
            bool GetRowAt(int yCoord, int & itemRow);
            bool GetColumnAt(int xCoord, int & itemColumn);
            bool GetItemRect(int x, int y, RECT & rect);

        private:
        
    };

}

#endif