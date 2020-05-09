#include "EditControl.h"
#include <SimpleIcu.h>
#include <type_traits>
#include <memory>

namespace WinLib {

    EditControl::EditControl() : isMultiLine(false), forwardArrowKeys(false), stopFowardingOnClick(false), autoExpand(false), hBuddy(NULL)
    {

    }

    EditControl::~EditControl()
    {

    }

    bool EditControl::FindThis(HWND hParent, u32 id)
    {
        return WindowControl::FindThis(hParent, id) &&
               WindowControl::RedirectProc();
    }

    bool EditControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool multiLine, u64 id)
    {
        DWORD dwStyle = WS_VISIBLE|WS_CHILD;
        if ( multiLine )
            dwStyle |= ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL|ES_WANTRETURN|WS_VSCROLL;
        else
            dwStyle |= ES_AUTOHSCROLL;

        isMultiLine = multiLine;

        return WindowControl::CreateControl( WS_EX_CLIENTEDGE, WC_EDIT, "", dwStyle,
                                             x, y, width, height,
                                             hParent, (HMENU)id, true );
    }

    bool EditControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool multiLine, bool clientEdge, u64 id)
    {
        DWORD dwStyle = WS_VISIBLE|WS_CHILD;
        if ( multiLine )
            dwStyle |= ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL|ES_WANTRETURN|WS_VSCROLL;
        else
            dwStyle |= ES_AUTOHSCROLL;

        isMultiLine = multiLine;

        DWORD dwExStyle = 0;
        if ( clientEdge )
            dwExStyle |= WS_EX_CLIENTEDGE;

        return WindowControl::CreateControl( dwExStyle, WC_EDIT, "", dwStyle,
                                             x, y, width, height,
                                             hParent, (HMENU)id, true );
    }

    void EditControl::SetForwardArrowKeys(bool forwardArrowKeyToParent)
    {
        forwardArrowKeys = forwardArrowKeyToParent;
    }

    void EditControl::SetStopForwardOnClick(bool stopForwardingKeysOnClick)
    {
        this->stopFowardingOnClick = stopForwardingKeysOnClick;
    }

    bool EditControl::SetText(const std::string & newText)
    {
        if ( getHandle() == ::GetFocus() )
        {
            DWORD selStart = 0, selEnd = 0;
            ::SendMessage(getHandle(), EM_GETSEL, (WPARAM)&selStart, (WPARAM)&selEnd);
            bool success = WindowsItem::SetWinText(newText);
            if ( success )
                ::SendMessage(getHandle(), EM_SETSEL, selStart, selEnd);

            return success;
        }
        else
            return WindowsItem::SetWinText(newText) != 0;
    }
    
    void EditControl::SetCaret(int caretPos)
    {
        ::SendMessage(getHandle(), EM_SETCARETINDEX, WPARAM(caretPos), 0);
    }

    bool EditControl::CreateNumberBuddy(int minimumValue, int maximumValue)
    {
        hBuddy = CreateWindowEx(0, UPDOWN_CLASS, NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_DISABLED |
            UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_HOTTRACK,
            0, 0, 0, 0, WindowsItem::getParent(), (HMENU)0, NULL, NULL);

        if ( hBuddy != NULL )
        {
            SendMessage(hBuddy, UDM_SETBUDDY, (WPARAM)getHandle(), 0);
            SendMessage(hBuddy, UDM_SETRANGE32, minimumValue, maximumValue);
        }
        return hBuddy != NULL;
    }

    void EditControl::EnableThis()
    {
        WindowsItem::EnableThis();

        if ( hBuddy != NULL )
            EnableWindow(hBuddy, TRUE);
    }

    void EditControl::DisableThis()
    {
        if ( hBuddy != NULL )
            EnableWindow(hBuddy, FALSE);

        WindowsItem::DisableThis();
    }

    template <typename numType>
    bool EditControl::SetEditNum(numType num)
    {
        return SetText(std::to_string(num));
    }
    template bool EditControl::SetEditNum<u8>(u8 num);
    template bool EditControl::SetEditNum<s8>(s8 num);
    template bool EditControl::SetEditNum<u16>(u16 num);
    template bool EditControl::SetEditNum<s16>(s16 num);
    template bool EditControl::SetEditNum<u32>(u32 num);
    template bool EditControl::SetEditNum<s32>(s32 num);
    template bool EditControl::SetEditNum<int>(int num);
    template bool EditControl::SetEditNum<size_t>(size_t num);

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
    template bool EditControl::SetEditBinaryNum<u8>(u8 num);
    template bool EditControl::SetEditBinaryNum<s8>(s8 num);
    template bool EditControl::SetEditBinaryNum<u16>(u16 num);
    template bool EditControl::SetEditBinaryNum<s16>(s16 num);
    template bool EditControl::SetEditBinaryNum<u32>(u32 num);
    template bool EditControl::SetEditBinaryNum<s32>(s32 num);
    template bool EditControl::SetEditBinaryNum<int>(int num);
    template bool EditControl::SetEditBinaryNum<size_t>(size_t num);

    void EditControl::SetTextLimit(u32 newLimit)
    {
        SendMessage(getHandle(), EM_SETLIMITTEXT, (WPARAM)newLimit, 0);
    }

    void EditControl::MaximizeTextLimit()
    {
        SendMessage(getHandle(), EM_SETLIMITTEXT, 0x7FFFFFFE, 0);
    }

    void EditControl::ExpandToText()
    {
        HDC hDC = GetDC(getHandle());
        std::string text;
        if ( hDC != NULL && GetWinText(text) && text.length() > 0 )
        {
            SIZE strSize = { };
            RECT textRect = { };
            if ( GetTextExtentPoint32(hDC, icux::toUistring(text).c_str(), GetTextLength(), &strSize) == TRUE &&
                 GetClientRect(getHandle(), &textRect) == TRUE &&
                 strSize.cx > (textRect.right-textRect.left) )
            {
                textRect.right = textRect.left + strSize.cx;
                if ( AdjustWindowRect(&textRect, GetWindowStyle(getHandle()), FALSE) != 0 )
                    SetWidth(textRect.right-textRect.left);
            }

            ReleaseDC(hDC);
        }
    }

    int EditControl::GetTextLength()
    {
        return WindowsItem::GetWinTextLen();
    }

    bool EditControl::GetEditBinaryNum(u16 & dest)
    {
        u16 temp = 0;
        std::string editText;
        if ( GetWinText(editText) && editText.length() > 0 )
        {
            const u16 u16BitValues[] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
                                         0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000 };

            size_t length = editText.length();
            for ( size_t i=length-1; i<length; i-- )
            {
                if ( editText[i] == '1' )
                    temp |= u16BitValues[(length-1)-i];
                else if ( editText[i] != '0' )
                    return false;
            }
            dest = temp;
            return true;
        }
        else
            return false;
    }

    bool EditControl::GetEditBinaryNum(u32 & dest)
    {
        u32 temp = 0;
        std::string editText;
        if ( GetWinText(editText) && editText.length() > 0 )
        {
            const u32 u32BitValues[] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
                                         0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000,
                                         0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000,
                                         0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000 };

            size_t length = editText.length();
            for ( size_t i=length-1; i<length; i-- )
            {
                if ( editText[i] == '1' )
                    temp |= u32BitValues[(length-1)-i];
                else if ( editText[i] != '0' )
                    return false;
            }
            dest = temp;
            return true;
        }
        else
            return false;
    }

    bool EditControl::GetHexByteString(u8* dest, u32 destLength)
    {
        std::string text;
        if ( GetWinText(text) && text.length() > 0 )
        {
            char strChunk[9] = { };
            strChunk[8] = '\0';

            size_t strLength = text.length();
            if ( strLength > size_t(destLength)*2 ) // Don't read past the number of requested bytes
                strLength = size_t(destLength)*2;

            size_t numChunks = strLength/8;
            size_t strRemainder = strLength%8;

            std::memset(dest, 0, destLength);
            for ( size_t chunk=0; chunk<numChunks; chunk++ ) // For all full chunks
            {
                for ( size_t i=0; i<8; i++ )
                    strChunk[i] = text[chunk*8+i];

                u32 result = strtoul(strChunk, nullptr, 16);
                dest[chunk*4  ] = u8( (result&0xFF000000) >> 24 );
                dest[chunk*4+1] = u8( (result&0x00FF0000) >> 16 );
                dest[chunk*4+2] = u8( (result&0x0000FF00) >>  8 );
                dest[chunk*4+3] = u8( (result&0x000000FF)       );
            }

            if ( strRemainder > 0 )
            {
                size_t strRemainderStart = numChunks*8;
                size_t destRemainderStart = numChunks*4;
                if ( strLength%2 == 0 )
                {
                    strChunk[strRemainder] = '\0';
                    for ( size_t i=0; i<strRemainder; i++ )
                        strChunk[i] = text[strRemainderStart+i];
                }
                else // Half a byte given, pad with a zero
                {
                    strChunk[strRemainder] = '0';
                    strChunk[strRemainder+1] = '\0';
                    for ( size_t i=0; i<strRemainder; i++ )
                        strChunk[i] = text[strRemainderStart+i];
                }
            
                u32 result = strtoul(strChunk, nullptr, 16);
                if ( strRemainder == 7 )
                {
                    dest[destRemainderStart  ] = u8( (result&0xFF000000) >> 24 );
                    dest[destRemainderStart+1] = u8( (result&0x00FF0000) >> 16 );
                    dest[destRemainderStart+2] = u8( (result&0x0000FF00) >>  8 );
                    dest[destRemainderStart+3] = u8( (result&0x000000FF)       );
                }
                if ( strRemainder == 5 || strRemainder == 6 )
                {
                    dest[destRemainderStart  ] = u8( (result&0xFF0000) >> 16 );
                    dest[destRemainderStart+1] = u8( (result&0x00FF00) >> 8 );
                    dest[destRemainderStart+2] = u8( (result&0x0000FF) );
                }
                else if (  strRemainder == 3 || strRemainder == 4 )
                {
                    dest[destRemainderStart  ] = u8( (result&0xFF00) >> 8 );
                    dest[destRemainderStart+1] = u8( (result&0x00FF) );
                }
                else if (  strRemainder == 1 || strRemainder == 2 )
                    dest[destRemainderStart] = u8(result);
            }

            return true;
        }
        else if ( GetTextLength() == 0 )
        {
            std::memset(dest, 0, destLength);
            return true;
        }
        else
            return false;
    }

    bool EditControl::SetHexByteString(u8* bytes, u32 numBytes)
    {
        std::unique_ptr<char> byteStringAlloc = std::unique_ptr<char>(new char[size_t(numBytes)*2+1]);
        char* byteString = byteStringAlloc.get();

        for ( u32 i=0; i<numBytes; i++ )
        {
            u8 firstChar = bytes[i]&0xF0;
            u32 firstCharPos = i*2;
            switch ( firstChar )
            {
                case 0x00: byteString[firstCharPos] = '0'; break;
                case 0x10: byteString[firstCharPos] = '1'; break;
                case 0x20: byteString[firstCharPos] = '2'; break;
                case 0x30: byteString[firstCharPos] = '3'; break;
                case 0x40: byteString[firstCharPos] = '4'; break;
                case 0x50: byteString[firstCharPos] = '5'; break;
                case 0x60: byteString[firstCharPos] = '6'; break;
                case 0x70: byteString[firstCharPos] = '7'; break;
                case 0x80: byteString[firstCharPos] = '8'; break;
                case 0x90: byteString[firstCharPos] = '9'; break;
                case 0xA0: byteString[firstCharPos] = 'A'; break;
                case 0xB0: byteString[firstCharPos] = 'B'; break;
                case 0xC0: byteString[firstCharPos] = 'C'; break;
                case 0xD0: byteString[firstCharPos] = 'D'; break;
                case 0xE0: byteString[firstCharPos] = 'E'; break;
                case 0xF0: byteString[firstCharPos] = 'F'; break;
            }

            u8 secondChar = bytes[i]&0x0F;
            u32 secondCharPos = i*2+1;
            switch ( secondChar )
            {
                case 0x00: byteString[secondCharPos] = '0'; break;
                case 0x01: byteString[secondCharPos] = '1'; break;
                case 0x02: byteString[secondCharPos] = '2'; break;
                case 0x03: byteString[secondCharPos] = '3'; break;
                case 0x04: byteString[secondCharPos] = '4'; break;
                case 0x05: byteString[secondCharPos] = '5'; break;
                case 0x06: byteString[secondCharPos] = '6'; break;
                case 0x07: byteString[secondCharPos] = '7'; break;
                case 0x08: byteString[secondCharPos] = '8'; break;
                case 0x09: byteString[secondCharPos] = '9'; break;
                case 0x0A: byteString[secondCharPos] = 'A'; break;
                case 0x0B: byteString[secondCharPos] = 'B'; break;
                case 0x0C: byteString[secondCharPos] = 'C'; break;
                case 0x0D: byteString[secondCharPos] = 'D'; break;
                case 0x0E: byteString[secondCharPos] = 'E'; break;
                case 0x0F: byteString[secondCharPos] = 'F'; break;
            }
        }
        byteString[numBytes*2] = '\0';
        SetText(byteString);
        return true;
    }

    template <typename numType>
    bool EditControl::GetEditNum(numType & dest)
    {
        std::string text;
        if ( GetWinText(text) && text.length() > 0 )
        {
            errno = 0;
            char* endPtr = nullptr;
            long long temp = std::strtoll(text.c_str(), &endPtr, 0);
            if ( temp != 0 )
            {
                dest = (numType)temp;
                return true;
            }
            else if ( errno == 0 && endPtr == &text[text.size()] )
            {
                dest = 0;
                return true;
            }
        }
        return false;
    }
    template bool EditControl::GetEditNum<u8>(u8 & dest);
    template bool EditControl::GetEditNum<s8>(s8 & dest);
    template bool EditControl::GetEditNum<u16>(u16 & dest);
    template bool EditControl::GetEditNum<s16>(s16 & dest);
    template bool EditControl::GetEditNum<u32>(u32 & dest);
    template bool EditControl::GetEditNum<s32>(s32 & dest);
    template bool EditControl::GetEditNum<int>(int & dest);

    LRESULT EditControl::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if ( msg == WM_LBUTTONDOWN && stopFowardingOnClick && forwardArrowKeys )
            forwardArrowKeys = false;
        else if ( msg == WM_KEYDOWN )
        {
            switch ( wParam )
            {
                case 'A':
                    if ( (GetKeyState(VK_CONTROL) & 0x8000) > 0 )
                        SendMessage(hWnd, EM_SETSEL, 0, -1);
                    break;
                case VK_TAB:
                    {
                        DWORD selStart, selEnd;
                        SendMessage(hWnd, EM_GETSEL, (WPARAM)&selStart, (LPARAM)&selEnd);
                        SendMessage(hWnd, EM_SETSEL, (WPARAM)selStart, (LPARAM)selEnd);
                        SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)"\t");
                        return 0; // Prevent default selection update
                    }
                    break;
                case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
                    if ( forwardArrowKeys )
                    {
                        SendMessage(GetParent(hWnd), WM_KEYDOWN, wParam, lParam);
                        return 0;
                    }
                    break;
            }

            if ( autoExpand )
                ExpandToText();
        }
        else if ( msg == WM_CHAR )
        {
            if ( (GetKeyState('A') & 0x8000) > 0 && (GetKeyState(VK_CONTROL) & 0x8000) > 0 ) // Prevent key-beep for select all
                return 0;
            else if ( forwardArrowKeys && (wParam == VK_LEFT || wParam == VK_UP || wParam == VK_RIGHT || wParam == VK_DOWN) )
                return 0;
        }
    
        return WindowControl::ControlProc(hWnd, msg, wParam, lParam); // Take default action
    }

}
