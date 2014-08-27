#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "Common Files/CommonFiles.h"
#include "Data.h"
#include "Clipboard.h"

/* Default Draw Order:
	- Final Tile Data (what MTXM will be)
	- Units & Sprites, highest YC to lowest YC
	  in the case of a tie, the highest index
	  is drawn first.
*/

class SpriteNode
	// Node for unit/sprite graphics
{
	public:

		u8 flags;
			#define GRAPHIC_ACTIVE		 0x1 // Marks the sprite for drawing
			#define GRAPHIC_PURE_SPRITE	 0x2 // Sprite not designated to be a unit
			#define GRAPHIC_UNIT_SPRITE	 0x4 // Sprite designated to be a unit
			#define GRAPHIC_UNIT		 0x8 // Sprite abstracted from unit

		u8 frame; // The current graphic frame of this unit
		u16 spriteDatEntry; // Pointer to dat entry defining this sprite
		u16 iScriptCmdNum; // Tracks current progress through this sprites iScript
		u16 id; // unitID or spriteID depending on the object this was made for

		void Deactivate() { flags &= 0xFE; } // Flips off 'active' flag
};

class Graphics
{
	public:

		Graphics(Scenario* chk) : displayingTileNums(false), tileNumsFromMTXM(false), displayingElevations(false), clipLocationNames(true), chk(chk) { }

		void DrawMap( u16 bitWidth, u16 bitHeight,
					  s32 screenLeft, s32 screenTop,
					  u8* screenBits, SELECTIONS& selections,
					  u32 layer, HDC hDC, bool showAnywhere );

		void DrawTerrain(u8* screenBits);
		void DrawTileElevations(u8* screenBits);
		void DrawGrid(u8* screenBits);
		void DrawLocations(u8* screenBits, SELECTIONS& selections, bool showAnywhere);
		void DrawUnits(u8* screenBits, SELECTIONS& selections);
		void DrawSprites(u8* screenBits);
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

		GRID& grid(u32 gridNum);

		/** Forces all SpriteNodes to be updated upon next draw
			Should be called whenever changes are made to
			unit or sprite data */
		void InvalidateSprites();


	private:

		Scenario* chk; // Pointer to the scenario file this instance of graphics renders

		bool isValid; // When false, requires complete sprite refresh
		bool isSorted; // When false, requires sorting before next use
		SpriteNode* spriteNodes; // Sorted ArrayList of spriteNodes

		HWND hPane; // Handle to destination window
		s32 screenLeft; // X-Position of the screens left edge in the map
		s32 screenTop; // Y-Position of the screens top edge in the map
		u32 screenWidth; // Width of screen
		u32 screenHeight; // Height of screen
		u16 mapWidth; // In tiles
		u16 mapHeight; // In tiles

		GRID grids[2]; // The grids drawn over the map
		bool tileNumsFromMTXM; // When false, tile nums are from TILE
		bool displayingTileNums; // Determines whether tile numbers are showing
		bool displayingElevations; // Determines whether tile elevations are colored in
		bool clipLocationNames; // Determines whether the locationName can be drawn partly outside locations

		// Utility Methods...
};

void GrpToBits( u8* screenBits, u16 &bitWidth, u16 &bitHeight, s32 &xStart, s32 &yStart, GRP* grp,
				u16 grpXC, u16 grpYC, u16 frame, buffer* palette, u8 color, bool flipped );

void UnitToBits( u8* screenBits, buffer* palette, u8 color, u16 bitWidth, u16 bitHeight,
				 s32 &xStart, s32 &yStart, u16 unitID, u16 unitXC, u16 unitYC, u16 frame, bool selected );

void SpriteToBits( u8* screenBits, buffer* palette, u8 color, u16 bitWidth, u16 bitHeight,
				   s32 &xStart, s32 &yStart, u16 spriteID, u16 spriteXC, u16 spriteYC );

void TileToBits(u8* screenBits, TileSet* tiles, s32 xStart, s32 yStart, u16 width, u16 height, u16 &TileValue);

void DrawMiniTileElevation(HDC hDC, TileSet* tiles, s16 xOffset, s16 yOffset, u16 tileValue, u8 miniTileX, u8 miniTileY, BITMAPINFO &bmi);

void DrawTileElevation(HDC hDC, TileSet* tiles, s16 xOffset, s16 yOffset, u16 tileValue, BITMAPINFO &bmi);

void TileElevationsToBits( u8* screenBits, u32 &bitWidth, u32 &bitHeight, TileSet* tiles,
						   s16 xOffset, s16 yOffset, u16 &TileValue, BITMAPINFO &bmi, u8 miniTileSeparation );

void DrawTile( HDC hDC, TileSet* tiles, s16 xOffset, s16 yOffset, u16 &TileValue,
			   BITMAPINFO &bmi, u8 BlueOffset, u8 GreenOffset, u8 RedOffset );

void DrawTileNumbers( HDC hDC, bool tileNumsFromMTXM, u32 screenLeft, u32 screenTop,
					  u16 xSize, u16 ySize, u16 bitHeight, u16 bitWidth, Scenario* chk );

void DrawTools( HDC hDC, HBITMAP tempBitmap, u16 width, u16 height, u32 screenLeft, u32 screenTop,
				SELECTIONS& selections, bool pasting, CLIPBOARD& clipboard, Scenario* chk, u8 layer );

void DrawTileSel(HDC hDC, u16 width, u16 height, u32 screenLeft, u32 screenTop, SELECTIONS& selections, Scenario* chk);

void DrawPasteGraphics( HDC hDC, HBITMAP tempBitmap, u16 width, u16 height, u32 screenLeft, u32 screenTop, SELECTIONS& selections,
					    CLIPBOARD& clipboard, Scenario* chk, u8 layer);

void DrawTempLocs(HDC hDC, u32 screenLeft, u32 screenTop, SELECTIONS& selections, Scenario* chk);

void DrawSelectingFrame(HDC hDC, SELECTIONS& selections, u32 screenLeft, u32 screenTop, s32 width, s32 height, double scale);

void DrawLocationFrame(HDC hDC, s32 left, s32 top, s32 right, s32 bottom);

void DrawMiniMap(HDC hDC, HWND hWnd, u16 xSize, u16 ySize, float scale, Scenario* chk);

void DrawMiniMapBox(HDC hDC, u32 screenLeft, u32 screenTop, u16 screenWidth, u16 screenHeight, u16 xSize, u16 ySize, float scale);

#define MAX_UNIT_LEFT 128
#define MAX_UNIT_RIGHT 127
#define MAX_UNIT_UP 80
#define MAX_UNIT_DOWN 79

#endif