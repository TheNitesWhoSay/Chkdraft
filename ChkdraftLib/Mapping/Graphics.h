#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "../CommonFiles/CommonFiles.h"
#include "../../WindowsLib/WindowsUi.h"
#include "Clipboard.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <array>
class GuiMap;

/* Default Draw Order:
    - Final Tile Data (what MTXM will be)
    - Units & Sprites, northmost to southmost, in the case of a tie, the highest index is drawn first. */

using ChkdBitmap = std::vector<Sc::SystemColor>;
using ChkdPalette = std::array<Sc::SystemColor, Sc::NumColors>;

extern Sc::SystemColor black;

class Graphics;

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
                No,
                Yes
            };

            Enabled enabled;
            u8 ticksBetweenRotations; // 
            u8 ticksRemaining; // When this reaches 0, palette colors from min to max are each rotated one index to the right, with the rightmost 
            u16 paletteIndexMin;
            u16 paletteIndexMax;
        };

        virtual ~ColorCycler();
        static bool CycleColors(const u16 tileset, ChkdPalette & palette); // Returns true if the map should be redrawn

    private:
        static constexpr size_t TotalRotatorSets = 4;
        static constexpr size_t MaxRotatersPerSet = 8;

        static DWORD prevTickCount; // Value from GetTickCount() -- updated every time GetTickCount increases (~16 ms)

        static const size_t TilesetRotationSet[Sc::Terrain::NumTilesets]; // Index of the rotater to use for a given tileset, all values must be less than TotalRotaterSets
        static Rotator NoRotators[MaxRotatersPerSet]; // An empty rotator set
        static Rotator RotatorSets[TotalRotatorSets][MaxRotatersPerSet]; // All rotator sets
};

class Graphics
{
    public:

        Graphics(GuiMap & map, Selections & selections);
        virtual ~Graphics();
        
        ChkdPalette & getPalette();
        void updatePalette();

        void DrawMap(const WinLib::DeviceContext & dc, u16 bitWidth, u16 bitHeight, s32 screenLeft, s32 screenTop, ChkdBitmap & bitmap, bool showAnywhere);

        void DrawTerrain(ChkdBitmap & bitmap);
        void DrawTileElevations(ChkdBitmap & bitmap);
        void DrawGrid(ChkdBitmap & bitmap);
        void DrawLocations(ChkdBitmap & bitmap, bool showAnywhere);
        void DrawUnits(ChkdBitmap & bitmap);
        void DrawSprites(ChkdBitmap & bitmap);
        void DrawSprite(ChkdBitmap & bitmap);
        void DrawLocationNames(const WinLib::DeviceContext & dc);
        void DrawIsomNumbers(const WinLib::DeviceContext & dc);
        void DrawTileNumbers(const WinLib::DeviceContext & dc);

        void AdjustSize(u32 newWidth, u32 newHeight); // Updates pane size and first and last sprite nodes
        void AdjustPosition(u32 newX, u32 newY); // Updates first and last sprite nodes

        void ToggleDisplayIsomValues();
        void ToggleTileNumSource(bool MTXMoverTILE);
        bool mtxmOverTile();
        bool DisplayingIsomNums();
        bool DisplayingTileNums();
        void ToggleLocationNameClip();
        bool ClippingLocationNames();

        void ToggleDisplayElevations();
        bool DisplayingElevations();

        bool GetGridSize(u32 gridNum, u16 & outWidth, u16 & outHeight);
        bool GetGridOffset(u32 gridNum, u16 & outX, u16 & outY);
        bool GetGridColor(u32 gridNum, u8 & red, u8 & green, u8 & blue);
        bool SetGridSize(u32 gridNum, u16 xSize, u16 ySize);
        bool SetGridColor(u32 gridNum, u8 red, u8 green, u8 blue);

        void DrawTools(const WinLib::DeviceContext & dc, u16 width, u16 height, u32 screenLeft, u32 screenTop,
            Selections & selections, bool pasting, Clipboard & clipboard, GuiMap & map);

        /** Forces all SpriteNodes to be updated upon next draw
            Should be called whenever changes are made to
            unit or sprite data */
        void InvalidateSprites();


    private:

        GuiMap & map; // Reference to the map this instance of graphics renders
        Selections & selections; // Reference to the selections belonging to the corresponding map
        ChkdPalette palette;

        s32 screenLeft; // X-Position of the screens left edge in the map
        s32 screenTop; // Y-Position of the screens top edge in the map
        s32 screenWidth; // Width of screen
        s32 screenHeight; // Height of screen
        u16 mapWidth; // In tiles
        u16 mapHeight; // In tiles

        MapGrid grids[2]; // The grids drawn over the map
        bool tileNumsFromMTXM; // When false, tile nums are from TILE
        bool displayingIsomTypes; // Determine whether ISOM terrain types are showing
        bool displayingTileNums; // Determines whether tile numbers are showing
        bool displayingElevations; // Determines whether tile elevations are colored in
        bool clipLocationNames; // Determines whether the locationName can be drawn partly outside locations

        // Utility Methods...
};

BITMAPINFO GetBMI(s32 width, s32 height);

void UnitToBits(ChkdBitmap & bitmap, ChkdPalette & palette, u8 color, u16 bitWidth, u16 bitHeight,
                 s32 & xStart, s32 & yStart, u16 unitID, u16 unitXC, u16 unitYC, u16 frame, bool selected );

void SpriteToBits(ChkdBitmap & bitmap, ChkdPalette & palette, u8 color, u16 bitWidth, u16 bitHeight,
                   s32 xStart, s32 yStart, u16 spriteID, u16 spriteXC, u16 spriteYC, bool flipped, bool selected);

void TileToBits(ChkdBitmap & bitmap, ChkdPalette & palette, const Sc::Terrain::Tiles & tiles, s64 xStart, s64 yStart, s64 width, s64 height, u16 TileValue);

void TileToBits(ChkdBitmap & bitmap, ChkdPalette & palette, const Sc::Terrain::Tiles & tiles, s64 xStart, s64 yStart, s64 width, s64 height, u16 TileValue,
    u8 redOffset, u8 blueOffset, u8 greenOffset);

void DrawMiniTileElevation(const WinLib::DeviceContext & dc, const Sc::Terrain::Tiles & tiles, s64 xOffset, s64 yOffset, u16 tileValue, s64 miniTileX, s64 miniTileY, BITMAPINFO & bmi);

void DrawTileElevation(const WinLib::DeviceContext & dc, const Sc::Terrain::Tiles & tiles, s16 xOffset, s16 yOffset, u16 tileValue, BITMAPINFO & bmi);

void TileElevationsToBits(ChkdBitmap & bitmap, s64 bitWidth, s64 bitHeight, const Sc::Terrain::Tiles & tiles,
                           s64 xOffset, s64 yOffset, u16 TileValue, BITMAPINFO & bmi, u8 miniTileSeparation );

void DrawTile(const WinLib::DeviceContext & dc, ChkdPalette & palette, const Sc::Terrain::Tiles & tiles, s16 xOffset, s16 yOffset, u16 TileValue, BITMAPINFO & bmi,
    u8 redOffset, u8 greenOffset, u8 blueOffset);

void DrawTools(const WinLib::DeviceContext & dc, ChkdPalette & palette, u16 width, u16 height, u32 screenLeft, u32 screenTop,
                Selections & selections, bool pasting, Clipboard & clipboard, GuiMap & map);

void DrawTileSel(const WinLib::DeviceContext & dc, ChkdPalette & palette, u16 width, u16 height, u32 screenLeft, u32 screenTop, Selections & selections, GuiMap & map);

void DrawDoodadSel(const WinLib::DeviceContext & dc, u16 width, u16 height, u32 screenLeft, u32 screenTop, Selections & selections, GuiMap & map);

void DrawPasteGraphics(const WinLib::DeviceContext & dc, ChkdPalette & palette, u16 width, u16 height, u32 screenLeft, u32 screenTop, Selections & selections,
                        Clipboard & clipboard, GuiMap & map, Layer layer, TerrainSubLayer terrainSubLayer);

void DrawTempLocs(const WinLib::DeviceContext & dc, u32 screenLeft, u32 screenTop, Selections & selections, GuiMap & map);

void DrawSelectingFrame(const WinLib::DeviceContext & dc, Selections & selections, u32 screenLeft, u32 screenTop, s32 width, s32 height, double scale);

void DrawLocationFrame(const WinLib::DeviceContext & dc, s32 left, s32 top, s32 right, s32 bottom);

void DrawMiniMap(const WinLib::DeviceContext & dc, const ChkdPalette & palette, u16 xSize, u16 ySize, float scale, GuiMap & map);

void DrawMiniMapBox(const WinLib::DeviceContext & dc, u32 screenLeft, u32 screenTop, u16 screenWidth, u16 screenHeight, u16 xSize, u16 ySize, float scale);

void DrawStringLine(const WinLib::DeviceContext & dc, UINT xPos, UINT yPos, LONG width, COLORREF defaultColor, std::string str);

bool GetStringDrawSize(const WinLib::DeviceContext & dc, UINT & width, UINT & height, const std::string & str);

bool GetStringDrawSize(const WinLib::DeviceContext & dc, UINT & width, UINT & height, const std::string & str, std::unordered_multimap<size_t, WinLib::LineSize> & lineCache);

void DrawString(const WinLib::DeviceContext & dc, UINT xPos, UINT yPos, LONG width, COLORREF defaultColor, std::string str);

#endif