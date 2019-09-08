#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "../CommonFiles/CommonFiles.h"
#include "Clipboard.h"
#include <string>
#include <vector>
#include <array>
class GuiMap;

/* Default Draw Order:
    - Final Tile Data (what MTXM will be)
    - Units & Sprites, northmost to southmost, in the case of a tie, the highest index is drawn first. */

using ChkdBitmap = std::vector<u32>;
using ChkdPalette = std::array<u32, 256>;

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
        /**
            A tileset has a set of between zero and eight rotators, after every tick (one change in GetTickCount: ~16ms), each rotator's ticksRemaining is decremented

            If a rotator's ticksRemaining reaches zero, palette colors from min to max are each rotated one index to the right "palette[index+1] = palette[index]",
            the rightmost entry "palette[paletteIndexMax]" wraps around to become the leftmost entry "palette[paletteIndexMin]"
            following this, ticksRemaining gets set to ticksBetweenRotations, and anything using the palette should be redrawn
        */
        class Rotator
        {
        public:
            enum class Enabled {
                No = 0,
                Yes = 1
            };

            Enabled enabled;
            u8 ticksBetweenRotations; // 
            u8 ticksRemaining; // When this reaches 0, palette colors from min to max are each rotated one index to the right, with the rightmost 
            u16 paletteIndexMin;
            u16 paletteIndexMax;
        };

        virtual ~ColorCycler();
        static bool CycleColors(const u16 tileset); // Returns true if the map should be redrawn

    private:
        static constexpr size_t TotalRotatorSets = 4;
        static constexpr size_t MaxRotatersPerSet = 8;

        static DWORD prevTickCount; // Value from GetTickCount() -- updated every time GetTickCount increases (~16 ms)

        static const size_t TilesetRotationSet[Sc::Terrain::MaxTilesets]; // Index of the rotater to use for a given tileset, all values must be less than TotalRotaterSets
        static Rotator NoRotators[MaxRotatersPerSet]; // An empty rotator set
        static Rotator RotatorSets[TotalRotatorSets][MaxRotatersPerSet]; // All rotator sets
};

class Graphics
{
    public:

        Graphics(GuiMap &map, Selections &selections) : map(map), selections(selections),
            displayingTileNums(false), tileNumsFromMTXM(false), displayingElevations(false), clipLocationNames(true) { }
        virtual ~Graphics();

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

void DrawStringChunk(HDC hDC, UINT xPos, UINT yPos, std::string str);

void DrawStringLine(HDC hDC, UINT xPos, UINT yPos, LONG width, COLORREF defaultColor, std::string str);

bool GetStringDrawSize(HDC hDC, UINT &width, UINT &height, std::string str);

void DrawString(HDC hDC, UINT xPos, UINT yPos, LONG width, COLORREF defaultColor, std::string str);

#endif