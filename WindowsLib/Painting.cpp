#include "Painting.h"
#include <SimpleIcu.h>

namespace WinLib {

    PaintBuffer::PaintBuffer() : width(0), height(0), memDc(NULL), parentDc(NULL), memBitmap(NULL)
    {

    }

    PaintBuffer::~PaintBuffer()
    {
        Clear();
    }

    void PaintBuffer::Clear()
    {
        if ( memBitmap != NULL )
            DeleteObject(memBitmap);

        if ( memDc != NULL )
            DeleteDC(memDc);

        width = 0;
        height = 0;

        parentDc = NULL;
    }

    HDC PaintBuffer::GetPaintDc()
    {
        return memDc;
    }

    HBITMAP PaintBuffer::GetPaintBitmap()
    {
        return memBitmap;
    }

    bool PaintBuffer::SetSize(HDC parentDc, int bufferWidth, int bufferHeight)
    {
        if ( PaintBuffer::parentDc == parentDc && PaintBuffer::width == bufferWidth && PaintBuffer::height == bufferHeight )
            return true;

        Clear();
        PaintBuffer::width = bufferWidth;
        PaintBuffer::height = bufferHeight;
        PaintBuffer::parentDc = parentDc;
        memDc = CreateCompatibleDC(parentDc);
        if ( memDc != NULL )
        {
            memBitmap = CreateCompatibleBitmap(parentDc, width, height);
            if ( memBitmap != NULL )
            {
                HGDIOBJ result = SelectObject(memDc, memBitmap);
                return result != NULL && result != HGDI_ERROR;
            }
        }
        Clear();
        return false;
    }

    bool PaintBuffer::CopyTo(HDC hdcDest)
    {
        return BitBlt(hdcDest, 0, 0, width, height, memDc, 0, 0, SRCCOPY) != 0;
    }


    PaintFontPtr PaintFont::createFont(int height, int width, const std::string &fontName)
    {
        PaintFontPtr paintFont = PaintFontPtr(new PaintFont());
        paintFont->hFont = CreateFont(height, width, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, icux::toFilestring(fontName).c_str());
        if ( paintFont ->hFont != NULL )
            return paintFont;
        else
            return nullptr;
    }

    HFONT PaintFont::getFont()
    {
        return hFont;
    }

    PaintFont::~PaintFont()
    {
        if ( hFont != NULL )
            DeleteObject(hFont);
    }

    PaintFont::PaintFont() : hFont(NULL)
    {
    
    }

    bool getTextExtent(HDC hdc, const std::string &text, s32 &width, s32 &height)
    {
        SIZE size;
        icux::uistring sysText = icux::toUistring(text);
        if ( GetTextExtentPoint32(hdc, sysText.c_str(), (int)sysText.length(), &size) != 0 )
        {
            width = size.cx;
            height = size.cy;
            return true;
        }
        else
            return false;
    }

    bool getTabTextExtent(HDC hdc, const std::string &text, s32 &width, s32 &height)
    {
        icux::uistring sysText = icux::toUistring(text);
        DWORD result = GetTabbedTextExtent(hdc, sysText.c_str(), (int)sysText.size(), 0, NULL);
        if ( result != 0 )
        {
            width = LOWORD(result);
            height = HIWORD(result);
            return true;
        }
        else
            return false;
    }

    bool drawText(HDC hdc, const std::string &text, int x, int y, RECT &rect, bool clipped, bool opaque)
    {
        icux::uistring sysText = icux::toUistring(text);

        UINT options = 0;
        if ( clipped )
            options |= ETO_CLIPPED;
        if ( opaque )
            options |= ETO_OPAQUE;

        return ExtTextOut(hdc, x, y, (clipped ? ETO_CLIPPED : 0), &rect, sysText.c_str(), (UINT)sysText.length(), 0) != 0;
    }

    bool drawTabbedText(HDC hdc, const std::string &text, int x, int y)
    {
        icux::uistring sysText = icux::toUistring(text);
        return TabbedTextOut(hdc, x, y, sysText.c_str(), (int)sysText.size(), 0, NULL, 0) != 0;
    }

}
