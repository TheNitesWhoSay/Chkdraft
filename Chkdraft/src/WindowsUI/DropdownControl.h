#ifndef DROPDOWNCONTROL_H
#define DROPDOWNCONTROL_H
#include "WindowControl.h"
#include <vector>

class DropdownControl : public WindowControl
{
public:
        bool CreateThis(HWND hParent, int x, int y, int width, int height, bool editable, bool alwaysList,
            u32 id, int numItems, const char** items, HFONT font);
        bool CreateThis(HWND hParent, int x, int y, int width, int height, bool editable, bool alwaysList,
            u32 id, const std::vector<std::string>& items, HFONT font);
        void SetSel(int index);
        void ClearEditSel();
        int GetSel();
        bool GetItemText(int index, std::string &dest);

        void ClearItems();
        bool AddItem(const std::string &item);

        template <typename numType> // Allowed types: u8, s8, u16, s16, u32, s32/int
            bool GetEditNum(numType &dest);

        template <typename numType> // Allowed types: u8, s8, u16, s16, u32, s32/int
            bool SetEditNum(numType num);
};

#endif