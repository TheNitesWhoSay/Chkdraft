#ifndef SCGRAPHICS_H
#define SCGRAPHICS_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include "clipboard.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <array>
class GuiMap;
class Selections;

/* Default Draw Order:
    - Final Tile Data (what MTXM will be)
    - Units & Sprites, northmost to southmost, in the case of a tie, the highest index is drawn first. */

using ChkdBitmap = std::vector<Sc::SystemColor>;
using ChkdPalette = std::array<Sc::SystemColor, Sc::NumColors>;

extern Sc::SystemColor black;

class Graphics
{
    public:

        Graphics(GuiMap & map, Selections & selections);
        virtual ~Graphics();
        
        ChkdPalette & getPalette();
        ChkdPalette & getStaticPalette();
        void updatePalette();

        void DrawMap(const WinLib::DeviceContext & dc, u16 bitWidth, u16 bitHeight, s32 screenLeft, s32 screenTop, ChkdBitmap & bitmap, bool showAnywhere);

        void DrawStars(ChkdBitmap & bitmap);
        void DrawTerrain(ChkdBitmap & bitmap);
        void DrawTileElevations(ChkdBitmap & bitmap);
        void DrawGrid(ChkdBitmap & bitmap);
        void DrawLocations(ChkdBitmap & bitmap, bool showAnywhere);
        void DrawFog(ChkdBitmap & bitmap);
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

        void ToggleDisplayBuildability();
        bool DisplayingBuildability();

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
        ChkdPalette staticPalette;

        s32 screenLeft; // X-Position of the screens left edge in the map
        s32 screenTop; // Y-Position of the screens top edge in the map
        s32 screenWidth; // Width of screen
        s32 screenHeight; // Height of screen
        u16 mapWidth; // In tiles
        u16 mapHeight; // In tiles

        MapGrid<Sc::SystemColor> grids[2]; // The grids drawn over the map
        bool tileNumsFromMTXM; // When false, tile nums are from TILE
        bool displayingIsomTypes; // Determine whether ISOM terrain types are showing
        bool displayingTileNums; // Determines whether tile numbers are showing
        bool displayingBuildability; // Determines whether tile buildability is being shown
        bool displayingElevations; // Determines whether tile elevations are colored in
        bool clipLocationNames; // Determines whether the locationName can be drawn partly outside locations

        // Utility Methods...
};

BITMAPINFO GetBMI(s32 width, s32 height);

void UnitToBits(ChkdBitmap & bitmap, ChkdPalette & palette, u8 color, u16 bitWidth, u16 bitHeight,
                 s32 & xStart, s32 & yStart, u16 unitID, u16 unitXC, u16 unitYC, u16 frame, bool selected, bool lifted = false, bool attached = false);

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

void DrawTools(Graphics & graphics, const WinLib::DeviceContext & dc, ChkdPalette & palette, ChkdPalette & staticPalette, u16 width, u16 height, u32 screenLeft, u32 screenTop,
                Selections & selections, bool pasting, Clipboard & clipboard, GuiMap & map);

void DrawTileBuildability(const WinLib::DeviceContext & dc, ChkdPalette & palette, u16 width, u16 height, u32 screenLeft, u32 screenTop, GuiMap & map);

void DrawTileSel(const WinLib::DeviceContext & dc, ChkdPalette & palette, u16 width, u16 height, u32 screenLeft, u32 screenTop, Selections & selections, GuiMap & map);

void DrawFogTileSel(const WinLib::DeviceContext & dc, ChkdPalette & palette, u16 width, u16 height, u32 screenLeft, u32 screenTop, Selections & selections, GuiMap & map);

void DrawDoodadSel(const WinLib::DeviceContext & dc, u16 width, u16 height, u32 screenLeft, u32 screenTop, Selections & selections, GuiMap & map);

void DrawPasteGraphics(const WinLib::DeviceContext & dc, ChkdPalette & palette, ChkdPalette & staticPalette, u16 width, u16 height, u32 screenLeft, u32 screenTop, Selections & selections,
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