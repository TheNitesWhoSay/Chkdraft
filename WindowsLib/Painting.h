#ifndef PAINTING_H
#define PAINTING_H
#include "../IcuLib/SimpleIcu.h"
#include "DataTypes.h"
#include <Windows.h>
#include <unordered_map>
#include <string>
#include <memory>

namespace WinLib {

    class PaintBuffer
    {
        public:
            PaintBuffer();
            virtual ~PaintBuffer();
            void Clear(); // Can call to save memory, but this will automatically be done upon resize/destructor
            HDC GetPaintDc();
            HBITMAP GetPaintBitmap();
            bool SetSize(HDC parentDc, int bufferWidth, int bufferHeight);
            bool CopyTo(HDC hdcDest);

        private:
            int width;
            int height;
            HDC memDc;
            HDC parentDc;
            HBITMAP memBitmap;
    };


    class PaintFont;
    using PaintFontPtr = std::shared_ptr<PaintFont>;
    class PaintFont
    {
        public:
            static PaintFontPtr createFont(int height, int width, const std::string & fontName);
            HFONT getFont();

            virtual ~PaintFont();

        private:

            HFONT hFont;

            PaintFont();
    };
    
    struct LineSize {
        icux::uistring str;
        s32 width;
        s32 height;

        static std::hash<icux::uistring> strHash;
    };

    bool getTextExtent(HDC hdc, const std::string & text, s32 & width, s32 & height);
    bool getTabTextExtent(HDC hdc, const std::string & text, s32 & width, s32 & height);
    bool getTabTextExtent(HDC hdc, const icux::codepoint* text, const size_t length, s32 & width, s32 & height);
    bool getTabTextExtent(HDC hdc, const icux::codepoint* text, const size_t length, s32 & width, s32 & height, std::unordered_multimap<size_t, LineSize> & lineCache);
    bool drawText(HDC hdc, const std::string & text, int x, int y, RECT & rect, bool clipped, bool opaque);
    bool drawTabbedText(HDC hdc, const std::string & text, int x, int y);

}

#endif