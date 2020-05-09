#ifndef DROPDOWNCONTROL_H
#define DROPDOWNCONTROL_H
#include "WindowControl.h"
#include <vector>

namespace WinLib {

    class DropdownControl : public WindowControl
    {
        public:
            virtual ~DropdownControl();
            bool CreateThis(HWND hParent, int x, int y, int width, int height, bool editable, bool alwaysList,
                u64 id, const std::vector<std::string> & items, HFONT font);
            void SetSel(int index);
            void ClearEditSel();
            int GetSel();
            bool GetItemText(int index, output_param std::string & dest);

            void ClearItems();
            bool AddItem(const std::string & item);

            template <typename numType> // Allowed types: u8, s8, u16, s16, u32, s32/int
                bool GetEditNum(numType & dest);

            template <typename numType> // Allowed types: u8, s8, u16, s16, u32, s32/int
                bool SetEditNum(numType num);
    };

}

#endif