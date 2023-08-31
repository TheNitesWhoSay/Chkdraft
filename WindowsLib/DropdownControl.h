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
                u64 id, const std::vector<std::string> & items);
            void SetSel(int index);
            void ClearEditSel();
            int GetSel();
            int GetSelData();
            bool GetItemText(int index, std::string & dest);
            int GetNumItems();

            void ClearItems();
            int AddItem(const std::string & item); // Returns 0-based index of new item, negative if there's an error
            int AddItem(const std::string & item, int data);
            int RemoveItem(int index);

            template <typename numType>
                bool GetEditNum(numType & dest);

            template <typename numType>
                bool SetEditNum(numType num);
    };

    template <typename numType>
    bool DropdownControl::GetEditNum(numType & dest)
    {
        auto text = GetWinText();
        if ( text && text->length() > 0 )
        {
            errno = 0;
            char* endPtr = nullptr;
            long long temp = std::strtoll(text->c_str(), &endPtr, 0);
            if ( temp != 0 )
            {
                dest = (numType)temp;
                return true;
            }
            else if ( errno == 0 && endPtr == &text.value()[text->size()] )
            {
                dest = 0;
                return true;
            }
        }
        return false;
    }

    template <typename numType>
    bool DropdownControl::SetEditNum(numType num)
    {
        return WindowsItem::SetWinText(std::to_string(num));
    }

}

#endif