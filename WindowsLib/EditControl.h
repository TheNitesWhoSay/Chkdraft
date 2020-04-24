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

            template <typename numType> // Allowed types: u8, s8, u16, s16, u32, s32/int
                bool SetEditNum(numType num);

            template <typename numType> // Allowed types: u8, s8, u16, s16, u32, s32/int
                bool SetEditBinaryNum(numType num);

            void SetTextLimit(u32 newLimit);
            void MaximizeTextLimit(); // Sets text limit to 0x7FFFFFFF
            void ExpandToText(); // Expands the edit control to show all the current text

            int GetTextLength();
            bool GetEditBinaryNum(u16 & dest);
            bool GetEditBinaryNum(u32 & dest);

            bool GetHexByteString(u8* dest, u32 destLength);
            bool SetHexByteString(u8* bytes, u32 numBytes);

            template <typename numType> // Allowed types: u8, s8, u16, s16, u32, s32/int
                bool GetEditNum(numType & dest);


        protected:
            LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // Used to map Select All to Ctrl+A

        private:
            bool isMultiLine;
            bool forwardArrowKeys;
            bool stopFowardingOnClick;
            bool autoExpand;
            HWND hBuddy; // If NULL, the control has no buddy
    };

}

#endif