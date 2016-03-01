#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "CommonFiles/CommonFiles.h"
#include "Clipboard.h"
#include "ScData.h"
#include <string>
#include <vector>
#include <array>
class GuiMap;

/* Default Draw Order:
    - Final Tile Data (what MTXM will be)
    - Units & Sprites, northmost to southmost, in the case of a tie, the highest index is drawn first. */

typedef std::vector<u32> ChkdBitmap;

struct colorcycle
{
    char enabled; // This record is used
    char steps; // How many frames to pause between cycling
    char timer; // Frame counter
    short start; // WPE index to cycle from
    short stop; // WPE index to cycle to
};

class ColorCycler
{
    public:
        static bool CycleColors(const u16 tileset); // Returns true if the map should be redrawn

    private:
        static DWORD ccticks; // GetTickCount -- updated every time GetTickCount increases (~16 ms)
        static colorcycle cctable[4][8]; // Different color cycling definition tables
        static const u8 cctilesets[8]; // Table to use for each tileset
};

class Graphics
{
    public:

        Graphics(GuiMap &map, Selections &selections) : map(map), selections(selections),
            displayingTileNums(false), tileNumsFromMTXM(false), displayingElevations(false), clipLocationNames(true) { }

        void DrawMap(u16 bitWidth, u16 bitHeight, s32 screenLeft, s32 screenTop, ChkdBitmap& bitmap, HDC hDC, bool showAnywhere);

        void DrawTerrain(ChkdBitmap& bitmap);
        void DrawTileElevations(ChkdBitmap& bitmap);
        void DrawGrid(ChkdBitmap& bitmap);
        void DrawLocations(ChkdBitmap& bitmap, bool showAnywhere);
        void DrawUnits(ChkdBitmap& bitmap);
        void DrawSprites(ChkdBitmap& bitmap);
        void DrawLocationNames(HDC hDC);
        void DrawTileNumbers(HDC hDC);

        void AdjustSize(u32 newWidth, u32 newHeight); // Updates pane size and first and last sprite nodes
        void AdjustPosition(u32 newX, u32 newY); // Updates first and last sprite nodes

        void ToggleTileNumSource(bool MTXMoverTILE);
        bool mtxmOverTile();
        bool DisplayingTileNums();
        void ToggleLocationNameClip();
        bool ClippingLocationNames();

        void ToggleDisplayElevations();
        bool DisplayingElevations();

        bool GetGridSize(u32 gridNum, u16 &outWidth, u16 &outHeight);
        bool GetGridOffset(u32 gridNum, u16 &outX, u16 &outY);
        bool GetGridColor(u32 gridNum, u8 &red, u8 &green, u8 &blue);
        bool SetGridSize(u32 gridNum, u16 xSize, u16 ySize);
        bool SetGridColor(u32 gridNum, u8 red, u8 green, u8 blue);

        /** Forces all SpriteNodes to be updated upon next draw
            Should be called whenever changes are made to
            unit or sprite data */
        void InvalidateSprites();


    private:

        GuiMap &map; // Reference to the map this instance of graphics renders
        Selections &selections; // Reference to the selections belonging to the corresponding map

        s32 screenLeft; // X-Position of the screens left edge in the map
        s32 screenTop; // Y-Position of the screens top edge in the map
        s32 screenWidth; // Width of screen
        s32 screenHeight; // Height of screen
        u16 mapWidth; // In tiles
        u16 mapHeight; // In tiles

        MapGrid grids[2]; // The grids drawn over the map
        bool tileNumsFromMTXM; // When false, tile nums are from TILE
        bool displayingTileNums; // Determines whether tile numbers are showing
        bool displayingElevations; // Determines whether tile elevations are colored in
        bool clipLocationNames; // Determines whether the locationName can be drawn partly outside locations

        // Utility Methods...
};

BITMAPINFO GetBMI(s32 width, s32 height);

void GrpToBits(ChkdBitmap& bitmap, u16 &bitWidth, u16 &bitHeight, s32 &xStart, s32 &yStart, GRP* grp,
                u16 grpXC, u16 grpYC, u16 frame, buffer* palette, u8 color, bool flipped );

void UnitToBits(ChkdBitmap& bitmap, buffer* palette, u8 color, u16 bitWidth, u16 bitHeight,
                 s32 &xStart, s32 &yStart, u16 unitID, u16 unitXC, u16 unitYC, u16 frame, bool selected );

void SpriteToBits(ChkdBitmap& bitmap, buffer* palette, u8 color, u16 bitWidth, u16 bitHeight,
                   s32 &xStart, s32 &yStart, u16 spriteID, u16 spriteXC, u16 spriteYC );

void TileToBits(ChkdBitmap& bitmap, TileSet* tiles, s32 xStart, s32 yStart, u16 width, u16 height, u16 TileValue);

void DrawMiniTileElevation(HDC hDC, TileSet* tiles, s16 xOffset, s16 yOffset, u16 tileValue, u8 miniTileX, u8 miniTileY, BITMAPINFO &bmi);

void DrawTileElevation(HDC hDC, TileSet* tiles, s16 xOffset, s16 yOffset, u16 tileValue, BITMAPINFO &bmi);

void TileElevationsToBits(ChkdBitmap& bitmap, s32 bitWidth, s32 bitHeight, TileSet* tiles,
                           s16 xOffset, s16 yOffset, u16 TileValue, BITMAPINFO &bmi, u8 miniTileSeparation );

void DrawTile( HDC hDC, TileSet* tiles, s16 xOffset, s16 yOffset, u16 &TileValue,
               BITMAPINFO &bmi, u8 redOffset, u8 greenOffset, u8 blueOffset );

void DrawTileNumbers( HDC hDC, bool tileNumsFromMTXM, u32 screenLeft, u32 screenTop,
                      u16 xSize, u16 ySize, u16 bitHeight, u16 bitWidth, GuiMap &map );

void DrawTools( HDC hDC, HBITMAP tempBitmap, u16 width, u16 height, u32 screenLeft, u32 screenTop,
                Selections& selections, bool pasting, Clipboard& clipboard, GuiMap &map);

void DrawTileSel(HDC hDC, u16 width, u16 height, u32 screenLeft, u32 screenTop, Selections& selections, GuiMap &map);

void DrawPasteGraphics( HDC hDC, HBITMAP tempBitmap, u16 width, u16 height, u32 screenLeft, u32 screenTop, Selections& selections,
                        Clipboard& clipboard, GuiMap &map, Layer layer);

void DrawTempLocs(HDC hDC, u32 screenLeft, u32 screenTop, Selections& selections, GuiMap &map);

void DrawSelectingFrame(HDC hDC, Selections& selections, u32 screenLeft, u32 screenTop, s32 width, s32 height, double scale);

void DrawLocationFrame(HDC hDC, s32 left, s32 top, s32 right, s32 bottom);

void DrawMiniMap(HDC hDC, u16 xSize, u16 ySize, float scale, GuiMap &map);

void DrawMiniMapBox(HDC hDC, u32 screenLeft, u32 screenTop, u16 screenWidth, u16 screenHeight, u16 xSize, u16 ySize, float scale);

bool GetLineDrawSize(HDC hDC, SIZE* strSize, std::string str);

void DrawStringChunk(HDC hDC, UINT xPos, UINT yPos, std::string str);

void DrawStringLine(HDC hDC, UINT xPos, UINT yPos, LONG width, COLORREF defaultColor, std::string str);

bool GetStringDrawSize(HDC hDC, UINT &width, UINT &height, std::string str);

void DrawString(HDC hDC, UINT xPos, UINT yPos, LONG width, COLORREF defaultColor, std::string str);

#endif