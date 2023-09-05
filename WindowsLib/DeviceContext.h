#ifndef DEVICECONTEXT_H
#define DEVICECONTEXT_H
#include "../CrossCutLib/SimpleIcu.h"
#include "ResourceManager.h"
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <Windows.h>

namespace WinLib {

    struct LineSize {
        icux::uistring str;
        s32 width;
        s32 height;

        inline static std::hash<icux::uistring> strHash;
    };

    class DeviceContext
    {
        HWND hWnd = NULL;
        HDC hDC = NULL;
        HDC parentDC = NULL; // Only used when using double-buffering
        HBITMAP memBitmap = NULL;
        RECT drawArea {};
        mutable HGDIOBJ originalObject = NULL;
        mutable HGDIOBJ originalParentObject = NULL;

        void unbuffer()
        {
            if ( originalObject != NULL )
            {
                ::SelectObject(hDC, originalObject);
                originalObject = NULL;
            }

            if ( memBitmap != NULL )
            {
                ::DeleteObject(memBitmap);
                memBitmap = NULL;
            }

            if ( hDC != NULL && parentDC != NULL ) // Double-buffered
            {
                ::DeleteDC(hDC);

                hDC = parentDC;
                originalObject = originalParentObject;
                parentDC = NULL;
                originalParentObject = NULL;
                memBitmap = NULL;
                drawArea = {};
            }
        }

        bool buffer(const RECT & area)
        {
            if ( hDC != NULL )
            {
                parentDC = hDC;
                originalParentObject = originalObject;
                originalObject = NULL;

                hDC = ::CreateCompatibleDC(parentDC);
                if ( hDC != NULL )
                {
                    memBitmap = ::CreateCompatibleBitmap(parentDC, area.right-area.left, area.bottom-area.top);
                    if ( memBitmap != NULL )
                    {
                        drawArea.left = area.left;
                        drawArea.top = area.top;
                        drawArea.right = area.right;
                        drawArea.bottom = area.bottom;
                        auto result = ::SelectObject(hDC, memBitmap);
                        if ( result != NULL && result != HGDI_ERROR )
                        {
                            originalObject = result;
                            return true;
                        }
                        else // Selecting memBitmap into hDC failed
                            ::DeleteObject(memBitmap);
                    }
                    ::DeleteDC(hDC);
                }

                hDC = parentDC;
                originalObject = originalParentObject;

                parentDC = NULL;
                originalParentObject = NULL;
                memBitmap = NULL;
                drawArea = {};
            }
            return false;
        }

        bool buffer(int width, int height)
        {
            return buffer(RECT{0, 0, width, height});
        }

        bool matchesDimensions(const RECT & area) const
        {
            return drawArea.left == area.left && drawArea.top == area.top && drawArea.right == area.right && drawArea.bottom == area.bottom;
        }

    public:
        DeviceContext() = default;
        DeviceContext(HDC hDC) : hDC(hDC) {}
        DeviceContext(HWND hWnd) : hWnd(hWnd), hDC(::GetDC(hWnd)) {
            if ( hDC == NULL )
                hWnd = NULL;
            else
                ::GetClientRect(hWnd, &drawArea);
        }
        DeviceContext(HWND hWnd, int width, int height) : hWnd(hWnd), hDC(::GetDC(hWnd)) {
            if ( hDC == NULL )
                hWnd = NULL;
            else
                setBuffer(width, height);
        }
        DeviceContext(HWND hWnd, const RECT & area) : hWnd(hWnd), hDC(::GetDC(hWnd)) {
            if ( hDC == NULL )
                hWnd = NULL;
            else
                setBuffer(area);
        }
        DeviceContext(const WinLib::DeviceContext &) = delete; // Copying most likely to result in use after release
        ~DeviceContext() {
            release();
        }

        operator bool() const {
            return hDC != NULL;
        }

        int getWidth() const { return drawArea.right - drawArea.left; }

        int getHeight() const { return drawArea.bottom - drawArea.top; }

        void release()
        {
            if ( originalObject != NULL )
            {
                ::SelectObject(hDC, originalObject);
                originalObject = NULL;
            }

            if ( originalParentObject != NULL )
            {
                ::SelectObject(parentDC, originalParentObject);
                originalParentObject = NULL;
            }

            if ( memBitmap != NULL )
                ::DeleteObject(memBitmap);

            if ( hDC != NULL )
            {
                if ( parentDC != NULL ) // Double-buffered
                {
                    ::DeleteDC(hDC);
                    if ( hWnd != NULL )
                        ::ReleaseDC(hWnd, parentDC);
                }
                else if ( hWnd != NULL )
                    ::ReleaseDC(hWnd, hDC);
            }

            hWnd = NULL;
            hDC = NULL;
            parentDC = NULL;
            memBitmap = NULL;
            drawArea = {};
        }

        HDC getDcHandle() const
        {
            return hDC;
        }

        bool selectObject(HGDIOBJ obj) const
        {
            if ( hDC != NULL )
            {
                if ( originalObject == NULL )
                {
                    auto result = ::SelectObject(hDC, obj);
                    if ( result != NULL && result != HGDI_ERROR )
                    {
                        originalObject = result;
                        return true;
                    }
                }
                else
                {
                    auto result = ::SelectObject(hDC, obj);
                    return result != NULL && result != HGDI_ERROR;
                }
            }
            return false;
        }

        bool flushBuffer() const
        {
            if ( parentDC != NULL )
            {
                if ( ::BitBlt(parentDC, drawArea.left, drawArea.top, this->getWidth(), this->getHeight(), hDC, 0, 0, SRCCOPY) != 0 )
                {
                    if ( hWnd != NULL )
                        ValidateRect(hWnd, &drawArea);
            
                    return true;
                }
            }
            return false;
        }

        bool flushBuffer(const WinLib::DeviceContext & target) const
        {
            if ( ::BitBlt(target.hDC, drawArea.left, drawArea.top, this->getWidth(), this->getHeight(), hDC, 0, 0, SRCCOPY) != 0 )
            {
                if ( target.hWnd != NULL && target.parentDC == NULL ) // BitBlt target was the final DC (target was not double buffered)
                    ValidateRect(target.hWnd, &drawArea);

                return true;
            }
            return false;
        }

        bool validateRect() const
        {
            if ( hWnd != NULL )
            {
                if ( drawArea.right>drawArea.left && drawArea.bottom>drawArea.top )
                    return ::ValidateRect(hWnd, &drawArea) != 0;
                else
                    return ::ValidateRect(hWnd, NULL) != 0;
            }
            return false;
        }

        bool setBuffer(const RECT & drawArea, HDC dc = NULL) // Sets up an in-memory buffer (for double buffering)
        {   
            if ( dc == NULL ) // Buffer the existing DC
            {
                if ( parentDC != NULL && matchesDimensions(drawArea) ) // Already appropriately double-buffered
                    return true;
                else // Already double-buffered, but dimensions changed
                    unbuffer();
            
                return buffer(drawArea);
            }
            else // if ( dc != NULL ) // Buffer using this new DC
            {
                if ( hDC == NULL ) // Presently empty
                {
                    hDC = dc;
                    return buffer(drawArea);
                }
                else if ( dc == parentDC && matchesDimensions(drawArea) ) // Already appropriately double-buffered
                    return true;
                else if ( parentDC != NULL ) // Already double-buffered, but DC or dimensions differ
                {
                    if ( dc != parentDC ) // DC differs
                    {
                        release();
                        hDC = dc;
                    }
                    else // Dimensions differ
                        unbuffer();

                    return buffer(drawArea);
                }
                else if ( dc == hDC ) // Given DC same as unbuffered DC
                    return buffer(drawArea);
                else // Given DC different from unbuffered DC
                {
                    release();
                    hDC = dc;
                    return buffer(drawArea);
                }
            }
            return false;
        }

        bool setBuffer(int width, int height, HDC dc = NULL) // Sets up an in-memory buffer (for double buffering)
        {   
            return setBuffer(RECT{0, 0, width, height}, dc);
        }


        bool setFont(int width, int height, std::string fontName) const
        {
            HFONT hFont = ResourceManager::getFont(width, height, fontName);
            return hFont != NULL && selectObject(hFont);
        }

        bool setDefaultFont() const
        {
            HFONT hFont = ResourceManager::getDefaultFont();
            return hFont != NULL && selectObject(hFont);
        }

        bool setPen(int iStyle, int cWidth, COLORREF color) const
        {
            HPEN hPen = ResourceManager::getPen(iStyle, cWidth, color);
            return hPen != NULL && selectObject(hPen);
        }

        std::optional<SIZE> getTextExtentPoint32(const icux::uistring & str) const
        {
            if ( hDC != NULL )
            {
                SIZE size {};
                if ( GetTextExtentPoint32(hDC, str.c_str(), int(str.size()), &size) != 0 )
                    return size;
            }
            return std::nullopt;
        }

        std::optional<SIZE> getTextExtentPoint32(const std::string & str) const
        {
            if ( hDC != NULL )
            {
                icux::uistring sysText = icux::toUistring(str);
                SIZE size {};
                if ( GetTextExtentPoint32(hDC, sysText.c_str(), int(str.size()), &size) != 0 )
                    return size;
            }
            return std::nullopt;
        }

        bool getTabTextExtent(const std::string & text, std::int32_t & width, std::int32_t & height) const
        {
            if ( text.length() > 0 )
            {
                icux::uistring sysText = icux::toUistring(text);
                DWORD result = GetTabbedTextExtent(hDC, sysText.c_str(), (int)sysText.size(), 0, NULL);
                if ( result != 0 )
                {
                    width = LOWORD(result);
                    height = HIWORD(result);
                    return true;
                }
            }
            else // Empty text
            {
                width = 0;
                height = 0;
            }
            return false;
        }

        bool getTabTextExtent(const icux::codepoint* text, const size_t length, std::int32_t & width, std::int32_t & height) const
        {
            if ( length > 0 )
            {
                DWORD result = GetTabbedTextExtent(hDC, text, (int)length, 0, NULL);
                if ( result != 0 )
                {
                    width = LOWORD(result);
                    height = HIWORD(result);
                    return true;
                }
            }
            else // Empty text
            {
                width = 0;
                height = 0;
            }
            return false;
        }

        bool getTabTextExtent(const icux::codepoint* text, const size_t length, std::int32_t & width, std::int32_t & height, std::unordered_multimap<size_t, WinLib::LineSize> & lineCache) const
        {
            if ( length > 0 )
            {
                icux::uistring uiString(text, length);
                size_t hash = WinLib::LineSize::strHash(uiString);
                auto matches = lineCache.equal_range(hash);
                if ( matches.first != lineCache.end() && matches.first->first == hash )
                {
                    for ( auto it = matches.first; it != matches.second; ++it )
                    {
                        WinLib::LineSize & lineSize = it->second;
                        if ( lineSize.str.compare(uiString) == 0 )
                        {
                            width = lineSize.width;
                            height = lineSize.height;
                            return true;
                        }
                    }
                }

                DWORD result = GetTabbedTextExtent(hDC, text, (int)length, 0, NULL);
                if ( result != 0 )
                {
                    width = LOWORD(result);
                    height = HIWORD(result);
                    WinLib::LineSize lineSize = { uiString, width, height };
                    lineCache.insert(std::pair<size_t, WinLib::LineSize>(hash, lineSize));
                    return true;
                }
            }
            else // Empty text
            {
                width = 0;
                height = 0;
            }
            return false;
        }

        bool fillRect(const RECT & rect, COLORREF color) const
        {
            HBRUSH hBrush = ResourceManager::getSolidBrush(color);
            return hBrush != NULL && ::FillRect(hDC, &rect, hBrush) != 0;
        }

        bool fillSysRect(const RECT & rect, int nIndex) const
        {
            HBRUSH hBrush = GetSysColorBrush(nIndex);
            return hBrush != NULL && ::FillRect(hDC, &rect, hBrush) != 0;
        }
    
        bool drawFocusRect(const RECT & rect) const
        {
            return DrawFocusRect(hDC, &rect) != 0;
        }

        bool frameRect(const RECT & rect, COLORREF color) const
        {
            HBRUSH hBrush = ResourceManager::getSolidBrush(color);
            return hBrush != NULL && ::FrameRect(hDC, &rect, hBrush) != 0;
        }

        int setBkMode(int mode) const
        {
            return SetBkMode(hDC, mode);
        }

        int setStretchBltMode(int mode) const
        {
            return SetStretchBltMode(hDC, mode);
        }

        COLORREF setBkColor(COLORREF color) const
        {
            return SetBkColor(hDC, color);
        }

        COLORREF setTextColor(COLORREF color) const
        {
            return SetTextColor(hDC, color);
        }
    
        inline bool drawText(const std::string & text, int x, int y, RECT & rect, bool clipped, bool opaque) const
        {
            icux::uistring sysText = icux::toUistring(text);

            UINT options = 0;
            if ( clipped )
                options |= ETO_CLIPPED;
            if ( opaque )
                options |= ETO_OPAQUE;

            return ExtTextOut(hDC, x, y, (clipped ? ETO_CLIPPED : 0), &rect, sysText.c_str(), (UINT)sysText.length(), 0) != 0;
        }

        bool drawTabbedText(const std::string & text, int x, int y) const
        {
            icux::uistring sysText = icux::toUistring(text);
            return TabbedTextOut(hDC, x, y, sysText.c_str(), (int)sysText.size(), 0, NULL, 0) != 0;
        }

        void drawWrappableText(const std::string & text, const RECT & textArea)
        {
            auto width = textArea.right-textArea.left;
            auto height = textArea.bottom-textArea.top;
            icux::uistring str = icux::toUistring(text);

            SIZE strSize = {};
            RECT nullRect = {};
            ::GetTextExtentPoint32(hDC, &str[0], (int)str.size(), &strSize);
            s32 lineHeight = strSize.cy;

            if ( strSize.cx < width )
                ::ExtTextOut(hDC, textArea.left, textArea.top, ETO_OPAQUE, &nullRect, &str[0], (UINT)str.length(), 0);
            else if ( height > lineHeight ) // Can word wrap
            {
                size_t lastCharPos = str.size() - 1;
                s32 prevBottom = textArea.top;

                while ( (textArea.top+height) - prevBottom > lineHeight && str.size() > 0 )
                {
                    // Binary search for the character length of this line
                    size_t floor = 0;
                    size_t ceil = str.size();
                    while ( ceil - 1 > floor )
                    {
                        lastCharPos = (ceil - floor) / 2 + floor;
                        ::GetTextExtentPoint32(hDC, &str[0], (int)lastCharPos, &strSize);
                        if ( strSize.cx > width )
                            ceil = lastCharPos;
                        else
                            floor = lastCharPos;
                    }
                    ::GetTextExtentPoint32(hDC, &str[0], (int)floor + 1, &strSize); // Correct last character if needed
                    if ( strSize.cx > width )
                        lastCharPos = floor;
                    else
                        lastCharPos = ceil;
                    // End binary search

                    ::ExtTextOut(hDC, textArea.left, prevBottom, ETO_OPAQUE, &nullRect, &str[0], (UINT)lastCharPos, 0);
                    //while ( str[lastCharPos] == ' ' || str[lastCharPos] == '\t' )
                    //    lastCharPos++;
                    str = str.substr(lastCharPos, str.size());
                    prevBottom += lineHeight;
                }
            }
        }

        std::uint16_t getTextWidth(const std::string & text) const
        {
            if ( text.length() > 0 )
            {
                icux::uistring sysText = icux::toUistring(text);
                DWORD result = GetTabbedTextExtent(hDC, sysText.c_str(), (int)sysText.size(), 0, NULL);
                if ( result != 0 )
                    return LOWORD(result);
            }
            return 0;
        }

        COLORREF getBkColor() const
        {
            return GetBkColor(hDC);
        }

        int setBitsToDevice(int xDest, int yDest, DWORD w, DWORD h, int xSrc, int ySrc, UINT StartScan, UINT cLines, const VOID* lpvBits, const BITMAPINFO* lpbmi) const
        {
            return ::SetDIBitsToDevice(hDC, xDest, yDest, w, h, xSrc, ySrc, StartScan, cLines, lpvBits, lpbmi, DIB_RGB_COLORS);
        }

        bool moveTo(int x, int y) const
        {
            return ::MoveToEx(hDC, x, y, NULL) != 0;
        }

        bool lineTo(int x, int y) const
        {
            return ::LineTo(hDC, x, y) != 0;
        }

        COLORREF setPixel(int x, int y, COLORREF color) const
        {
            return ::SetPixel(hDC, x, y, color);
        }

        int getDiBits(UINT start, UINT cLines, LPVOID lpvBits, LPBITMAPINFO lpbmi) const
        {
            return ::GetDIBits(hDC, memBitmap, start, cLines, lpvBits, lpbmi, DIB_RGB_COLORS);
        }

        bool bitBlt(HDC dest, int x, int y, int cx, int cy, int x1, int y1, DWORD rop)
        {
            return ::BitBlt(dest, x, y, cx, cy, hDC, x1, y1, rop) != 0;
        }

        bool copySelfToBuffer() const
        {
            return ::BitBlt(hDC, drawArea.left, drawArea.top, drawArea.right-drawArea.left, drawArea.bottom-drawArea.top, parentDC, drawArea.left, drawArea.top, SRCCOPY) != 0;
        }

        bool stretchBlt(const DeviceContext & dest, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, int wSrc, int hSrc, DWORD rop) const
        {
            return ::StretchBlt(dest.getDcHandle(), xDest, yDest, wDest, hDest, hDC, xSrc, ySrc, wSrc, hSrc, rop);
        }

        bool drawIconEx(int xLeft, int yTop, HICON hIcon, int cxWidth, int cyWidth, UINT istepIfAniCur, HBRUSH hbrFlickerFreeDraw, UINT diFlags) const
        {
            return ::DrawIconEx(getDcHandle(), xLeft, yTop, hIcon, cxWidth, cyWidth, istepIfAniCur, hbrFlickerFreeDraw, diFlags) != 0;
        }
    };

}

#endif