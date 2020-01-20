#ifndef LISTBOXCONTROL_H
#define LISTBOXCONTROL_H
#include "WindowControl.h"
#include <queue>

namespace WinLib {

    /** May use some or all of of the LB:: messages
        from Enumerations.h */
    class ListBoxControl : public WindowControl
    {
        public:
            ListBoxControl();
            virtual ~ListBoxControl();

            // Attempts to create a list box
            bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool ownerDrawn, bool multiColumn, bool scrollBar, bool alphaSort, u64 id);
            void ClearItems();
            int AddItem(u32 item);
            int AddString(const std::string & str);
            int AddStrings(const std::vector<std::string> & strs);
            bool InsertString(int index, const std::string & str);
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
            bool GetItemHeight(int index, int & height);
            int ColumnWidth();
            bool GetCurSel(int & sel); // Used with single-selection list boxes
            int GetNumSel(); // Gets the number of items selected (in a multi-selection list box)
            bool GetSelString(int n, output_param std::string & str); // Attempts to get the nth selected item's string (in a multi-selection list box)
            bool GetCurSelString(output_param std::string & str); // Attempts to get the currently selected item's string
            bool GetSelItem(int n, int & itemData); // Attempts to get the nth selected item's data (in a multi-selection list box)
            bool GetItemData(int index, u32 & data);
            int GetTopIndex();

        protected:
            virtual LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        private:
            bool autoRedraw;
            std::queue<u32> itemsToAdd;
    };

}

#endif