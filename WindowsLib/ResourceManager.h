#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H
#include "../CrossCutLib/SimpleIcu.h"
#include <unordered_map>
#include <Windows.h>

namespace WinLib
{
    struct FontInfo
    {
        int width;
        int height;
        std::string name;

        friend constexpr bool operator==(const FontInfo & l, const FontInfo & r)
        {
            return l.width == r.width && l.height == r.height && l.name == r.name;
        }
    };

    struct PenInfo
    {
        int style;
        int width;
        COLORREF color;

        friend constexpr bool operator==(const PenInfo & l, const PenInfo & r)
        {
            return l.style == r.style && l.width == r.width && l.color == r.color;
        }
    };

    struct BitmapInfo
    {
        WORD resourceId;
        int width;
        int height;
        UINT fuLoad;

        friend constexpr bool operator==(const BitmapInfo & l, const BitmapInfo & r)
        {
            return l.resourceId == r.resourceId && l.width == r.width && l.height == r.height && l.fuLoad == r.fuLoad;
        }
    };

    struct IconInfo
    {
        WORD resourceId;
        int width;
        int height;
        UINT fuLoad;

        friend constexpr bool operator==(const IconInfo & l, const IconInfo & r)
        {
            return l.resourceId == r.resourceId && l.width == r.width && l.height == r.height && l.fuLoad == r.fuLoad;
        }
    };
}

template <> struct std::hash<WinLib::FontInfo>
{
    size_t operator()(const WinLib::FontInfo & fontInfo) const noexcept
    {
        return std::hash<std::string>{}(fontInfo.name) ^ (((size_t(fontInfo.width) << 8) + fontInfo.height) << 1);
    }
};

template <> struct std::hash<WinLib::PenInfo>
{
    size_t operator()(const WinLib::PenInfo & penInfo) const noexcept
    {
        return penInfo.color ^ (((size_t(penInfo.width) << 8) + penInfo.style) << 1);
    }
};

template <> struct std::hash<WinLib::BitmapInfo>
{
    size_t operator()(const WinLib::BitmapInfo & bitmapInfo) const noexcept
    {
        return (((size_t(bitmapInfo.width) << 8) + bitmapInfo.height) << 1) + bitmapInfo.fuLoad + bitmapInfo.resourceId;
    }
};

template <> struct std::hash<WinLib::IconInfo>
{
    size_t operator()(const WinLib::IconInfo & iconInfo) const noexcept
    {
        return (((((size_t(iconInfo.width) << 8) + iconInfo.height) << 1) + iconInfo.resourceId) << 1) + iconInfo.fuLoad;
    }
};

namespace WinLib
{
    struct PaintFont
    {
        HFONT hFont = NULL;

        PaintFont(const FontInfo & fontInfo)
            : hFont(CreateFont(fontInfo.height, fontInfo.width, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, icux::toFilestring(fontInfo.name).c_str())) {}
        ~PaintFont() { if ( hFont != NULL ) ::DeleteObject(hFont); }
    };

    struct SolidBrush
    {
        HBRUSH hBrush = NULL;

        SolidBrush(COLORREF color) : hBrush(::CreateSolidBrush(color)) {}
        ~SolidBrush() { if ( hBrush != NULL ) ::DeleteObject(hBrush); }
    };

    struct Pen
    {
        HPEN hPen = NULL;

        Pen(const PenInfo & penInfo) : hPen(::CreatePen(penInfo.style, penInfo.width, penInfo.color)) {}
        ~Pen() { if ( hPen != NULL ) ::DeleteObject(hPen); }
    };
    
    struct Bitmap
    {
        HBITMAP hImage = NULL;
        
        Bitmap(WORD resourceId)
            : hImage((HBITMAP)LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(resourceId))) {}
        Bitmap(const BitmapInfo & info)
            : hImage((HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(info.resourceId), IMAGE_BITMAP, info.width, info.height, info.fuLoad)) {}
        ~Bitmap() { if ( hImage != NULL ) ::DeleteObject(hImage); }
    };

    struct Icon
    {
        HICON hImage = NULL;

        Icon(const IconInfo & info)
            : hImage((HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(info.resourceId), IMAGE_ICON, info.width, info.height, info.fuLoad)) {}
        ~Icon() { if ( hImage != NULL ) ::DestroyIcon((HICON)hImage); }
    };

    class ResourceManager // Used to manage resources that hang around for the programs lifetime, automatically prevents creating the same resource twice
    {
        static inline std::unordered_map<COLORREF, SolidBrush> solidBrushes{};
        static inline std::unordered_map<PenInfo, Pen> pens{};
        static inline std::unordered_map<FontInfo, PaintFont> fonts{};
        static inline std::unordered_map<BitmapInfo, Bitmap> bitmaps{};
        static inline std::unordered_map<WORD, Bitmap> simpleBitmaps{};
        static inline std::unordered_map<IconInfo, Icon> icons{};

    public:

        static HBRUSH getSolidBrush(COLORREF color)
        {
            auto found = solidBrushes.find(color);
            if ( found != solidBrushes.end() )
                return found->second.hBrush;
        
            auto insertion = solidBrushes.try_emplace(color, color);
            return insertion.second ? insertion.first->second.hBrush : NULL;
        }

        static HPEN getPen(int iStyle, int cWidth, COLORREF color)
        {
            PenInfo penInfo {iStyle, cWidth, color};
            auto found = pens.find(penInfo);
            if ( found != pens.end() )
                return found->second.hPen;
        
            auto insertion = pens.try_emplace(penInfo, penInfo);
            return insertion.second ? insertion.first->second.hPen : NULL;
        }

        static HFONT getFont(int width, int height, const std::string & fontName)
        {
            FontInfo fontInfo { width, height, fontName };
            auto found = fonts.find(fontInfo);
            if ( found != fonts.end() )
                return found->second.hFont;
        
            auto insertion = fonts.try_emplace(fontInfo, fontInfo);
            return insertion.second ? insertion.first->second.hFont : NULL;
        }

        static HFONT getDefaultFont()
        {
            return getFont(5, 14, "Microsoft Sans Serif");
        }

        static HBITMAP getBitmap(WORD resourceId)
        {
            auto found = simpleBitmaps.find(resourceId);
            if ( found != simpleBitmaps.end() )
                return found->second.hImage;
        
            auto insertion = simpleBitmaps.try_emplace(resourceId, resourceId);
            return insertion.second ? insertion.first->second.hImage : NULL;
        }

        static HBITMAP getBitmap(WORD resourceId, int width, int height, UINT fuLoad)
        {
            BitmapInfo bitmapInfo { resourceId, width, height, fuLoad };
            auto found = bitmaps.find(bitmapInfo);
            if ( found != bitmaps.end() )
                return found->second.hImage;
        
            auto insertion = bitmaps.try_emplace(bitmapInfo, bitmapInfo);
            return insertion.second ? insertion.first->second.hImage : NULL;
        }

        static HICON getIcon(WORD resourceId, int width, int height, UINT fuLoad = 0)
        {
            IconInfo iconInfo { resourceId, width, height, fuLoad };
            auto found = icons.find(iconInfo);
            if ( found != icons.end() )
                return found->second.hImage;
        
            auto insertion = icons.try_emplace(iconInfo, iconInfo);
            return insertion.second ? insertion.first->second.hImage : NULL;
        }

        static HCURSOR getCursor(
            #ifdef WINDOWS_UTF16
            LPCWSTR lpCursorName
            #else
            LPCSTR lpCursorName
            #endif
            )
        {
            return LoadCursor(NULL, lpCursorName);
        }
    };
}

#endif