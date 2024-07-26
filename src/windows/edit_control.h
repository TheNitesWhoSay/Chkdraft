#ifndef EDITCONTROL_H
#define EDITCONTROL_H
#include "WindowControl.h"

namespace WinLib {

    class EditControl : public WindowControl
    /** A basic edit control
        This EditControl class maps Select All to Ctrl+A by default */
    {
        public:
            EditControl();
            virtual ~EditControl();
            bool FindThis(HWND hParent, u32 controlId); // Attempts to encapsulate an edit control and enable Select All
            bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool multiLine, u64 id); // Attempts to create an edit control
            bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool multiLine, bool clientEdge, u64 id); // Attempts to create an edit control
            void SetForwardArrowKeys(bool forwardArrowKeyToParent); // Sets whether this control forwards arrow-keypresses to the parent
            void SetStopForwardOnClick(bool stopFowardingOnClick); // Sets whether this stops forwarding arrow-keys when clicked
            bool CreateNumberBuddy(int minimumValue, int maximumValue);
            virtual void EnableThis();
            virtual void DisableThis();

            bool SetText(const std::string & newText); // Sets new text content
            void SetCaret(int caretPos);

            template <typename numType>
                bool SetEditNum(numType num);

            template <typename numType>
                bool SetEditBinaryNum(numType num);

            void SetTextLimit(u32 newLimit);
            void MaximizeTextLimit(); // Sets text limit to 0x7FFFFFFF
            void ExpandToText(); // Expands the edit control to show all the current text

            int GetTextLength();
            bool GetEditBinaryNum(u16 & dest);
            bool GetEditBinaryNum(u32 & dest);

            bool GetHexByteString(u8* dest, u32 destLength);
            bool SetHexByteString(u8* bytes, u32 numBytes);

            template <typename numType>
            bool GetEditNum(numType & dest);

            template <typename NumType>
            std::optional<NumType> GetEditNum()
            {
                auto text = GetWinText();
                if ( text && text->length() > 0 )
                {
                    errno = 0;
                    char* endPtr = nullptr;
                    long long temp = std::strtoll(text->c_str(), &endPtr, 0);
                    if ( temp != 0 )
                        return NumType(temp);
                    else if ( errno == 0 && endPtr == &text.value()[text->size()] )
                        return NumType(0);
                }
                return std::nullopt;
            }


        protected:
            LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // Used to map Select All to Ctrl+A

        private:
            bool isMultiLine;
            bool forwardArrowKeys;
            bool stopFowardingOnClick;
            bool autoExpand;
            HWND hBuddy; // If NULL, the control has no buddy
    };

    template <typename numType>
    bool WinLib::EditControl::SetEditNum(numType num)
    {
        return SetText(std::to_string(num));
    }
    

    template <typename numType>
    bool EditControl::SetEditBinaryNum(numType num)
    {
        char newText[36] = { };
        u32 temp = (u32)num;
        u8 numBits = (sizeof(numType)*8);
        _itoa_s(temp, newText, 36, 2);
        size_t length = std::strlen(newText);
        if ( length > 0 && length < numBits && numBits < 36 )
        {
            std::memmove(&newText[numBits-length], newText, length);
            std::memset(newText, '0', numBits-length);
            newText[numBits] = '\0';
        }
        return SetText(newText);
    }

    template <typename numType>
    bool EditControl::GetEditNum(numType & dest)
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

}

#endif