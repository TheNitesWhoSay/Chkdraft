#ifndef GUIMAP_H
#define GUIMAP_H
#include "Common Files/CommonFiles.h"
#include "Windows UI/WindowsUI.h"
#include "Clipboard.h"
#include "Undo.h"
#include "Data.h"
#include "Graphics.h"

#define GuiMapPtr std::shared_ptr<GuiMap>

class GuiMap : public MapFile, public ClassWindow, public IObserveUndos
{
	public:

/* Constructor	*/	GuiMap();

/*  Destructor  */	~GuiMap();

/*	  File IO	*/	bool CanExit();
					bool SaveFile(bool saveAs);

/*   Chk Accel  */	// Sets the given tile to the given tileNum
					bool SetTile(s32 x, s32 y, u16 tileNum);

/*   UI Accel   */	u8& currLayer();
					double getZoom();
					void setZoom(double newScale);
					u8& currPlayer();
					bool isDragging();
					void setDragging(bool dragging);

					void viewLocation(u16 index);
					u8 getLocSelFlags(s32 xc, s32 yc);
					bool moveLocation(u32 downX, u32 downY, u32 upX, u32 upY);

					void doubleClickLocation(s32 xClick, s32 yClick);
					void openTileProperties(s32 xClick, s32 yClick);
					void EdgeDrag(HWND hWnd, int x, int y, u8 layer);

					u8 getDisplayOwner(u8 player); // Returns player owner index for dialogs

					void refreshScenario();

/*   Sel/Paste  */  void clearSelection();
					void selectAll();
					void deleteSelection();
					SELECTIONS& selections() { return selection; };
					void paste(s32 mapClickX, s32 mapClickY, CLIPBOARD &clipboard);

/*   Undo Redo  */  UNDOS& undos() { return undoStacks; }
					void undo();
					void redo();
					virtual void ChangesMade();
					virtual void ChangesReversed();

/*   Graphics   */	POINT& display() { return displayPivot; }
					GRID& grid(u32 gridNum) { return graphics.grid(gridNum); };
					float MiniMapScale(u16 xSize, u16 ySize);
				
					HWND getHandle();

					void PaintMap(GuiMapPtr currMap, bool pasting, CLIPBOARD& clipboard);
					void PaintMiniMap(HWND hWnd);
					void Redraw(bool includeMiniMap);
					void ValidateBorder(s32 screenWidth, s32 screenHeight);

					void SetGrid(s16 xSize, s16 ySize);
					void SetGridColor(u8 red, u8 green, u8 blue);

					void ToggleDisplayElevations();
					bool DisplayingElevations();

					bool snapUnitCoordinate(s32& x, s32& y);

					void ToggleUnitSnap();
					void ToggleUnitStack();

					void ToggleTileNumSource(bool MTXMoverTILE);
					bool DisplayingTileNums();

					void ToggleLocationNameClip();
					void SetLocationSnap(u32 flags);
						#define SNAP_LOCATION_TO_TILE BIT_0
						#define SNAP_LOCATION_TO_GRID BIT_1
						#define NO_LOCATION_SNAP BIT_2
					void ToggleLockAnywhere();
					bool LockAnywhere();

					bool GetSnapIntervals(u32& x, u32& y, u32& xOffset, u32& yOffset);
					bool SnapLocationDimensions(s32& x1, s32& y1, s32& x2, s32& y2, u32 flags);
						#define SNAP_LOC_X1 BIT_0
						#define SNAP_LOC_Y1 BIT_1
						#define SNAP_LOC_X2 BIT_3
						#define SNAP_LOC_Y2 BIT_4
						#define SNAP_LOC_ALL (SNAP_LOC_X1|SNAP_LOC_Y1|SNAP_LOC_X2|SNAP_LOC_Y2)

					void UpdateLocationMenuItems();
					void UpdateZoomMenuItems();
					void UpdateGridSizesMenu();
					void UpdateGridColorMenu();
					void UpdateTerrainViewMenuItems();
					void UpdateUnitMenuItems();

					void Scroll(u32 flags);
						#define SCROLL_X		0x1
						#define SCROLL_Y		0x2
						#define VALIDATE_BORDER 0x4

					u8* GraphicBits() { return lpvBits; }
					u16& bitWidth() { return bitMapWidth; }
					u16& bitHeight() { return bitMapHeight; }
					HDC GetMemhDC() { return MemhDC; }
					HDC GetMemMinihDC() { return MemMinihDC; }

/*	   Misc  	*/	void setMapId(u16 mapId);
					u16 getMapId();
					void notifyChange(bool undoable); // Notifies that a change occured, if it's not undoable changes are locked
					void changesUndone(); // Notifies that all undoable changes have been undone
					bool changesLocked(); // Checks if changes are locked
					void addAsterisk(); // Adds an asterix onto the map name
					void removeAsterisk(); // Removes an asterix from the map name
					void updateMenu(); // Updates which items are checked in the main menu

					bool CreateThis(HWND hClient, const char* title);
					void ReturnKeyPress();

					static CLIPBOARD* clipboard; // Get this pointing correctly...
					//static GuiMapPtr currMap; // Get this pointing correctly...

	private:

/*	   Data  	*/	u16 mapId;
					bool changeLock;
					bool unsavedChanges;

					u8 layer;
					u8 player;
					double zoom;

					bool dragging;

					bool snapUnits;
					bool stackUnits;

					bool snapLocations;
					bool locSnapTileOverGrid;
					bool lockAnywhere;

					SELECTIONS selection;
					UNDOS undoStacks;

					Graphics graphics;
					POINT displayPivot;

					u8* lpvBits;
					u16 bitMapWidth;
					u16 bitMapHeight;
					bool RedrawMiniMap;
					bool RedrawMap;
					HDC MemhDC;
					HDC MemMinihDC;
					HBITMAP MemBitmap;
					HBITMAP MemMiniBitmap;

					LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
					LRESULT MapMouseProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
					LRESULT HorizontalScroll(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
					LRESULT VerticalScroll(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
					LRESULT DoSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
					void ActivateMap(HWND hWnd);
					LRESULT DestroyWindow(HWND hWnd);

					void RButtonUp();
					void LButtonDoubleClick(int x, int y);
					void LButtonDown(int x, int y, WPARAM wParam);
					void MouseMove(HWND hWnd, int x, int y, WPARAM wParam);
					void MouseHover(HWND hWnd, int x, int y, WPARAM wParam);
					void LButtonUp(HWND hWnd, int x, int y, WPARAM wParam);
					void TerrainLButtonUp(HWND hWnd, int mapX, int mapY, WPARAM wParam);
					void LocationLButtonUp(HWND hWnd, int mapX, int mapY, WPARAM wParam);
					void UnitLButtonUp(HWND hWnd, int mapX, int mapY, WPARAM wParam);
					LRESULT ConfirmWindowClose(HWND hWnd);
};

/** Attempts to turn an escaped string (either via <XX>'s or \'s) into a raw string
	Returns true and modifies str if successful, str not modified otherwise */
bool parseEscapedString(std::string& str);

#endif