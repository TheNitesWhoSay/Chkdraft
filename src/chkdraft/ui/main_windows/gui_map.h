#ifndef GUIMAP_H
#define GUIMAP_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include "mapping/clipboard.h"
#include "mapping/map_animations.h"
#include "mapping/sc_graphics.h"
#include "mapping/selections.h"

namespace Scr { class MapGraphics; }

class GuiMap;
typedef std::shared_ptr<GuiMap> GuiMapPtr;
enum class DefaultTriggers
{
    NoTriggers = 0,
    DefaultMelee = 1,
    TwoPlayerMeleeWithObs = 2,
    ThreePlayerMeleeWithObs = 3,
    FourPlayerMeleeWithObs = 4,
    FivePlayerMeleeWithObs = 5,
    SixPlayerMeleeWithObs = 6,
    SevenPlayerMeleeWithObs = 7
};

enum class Direction
{
    Left,
    Up,
    Right,
    Down
};

class GuiMap : public MapFile, public WinLib::ClassWindow, private Chk::IsomCache
{
    public:
/* Public Data  */  Clipboard & clipboard;
                    Selections selections {*this};
                    std::unique_ptr<Scr::MapGraphics> scrGraphics;
                    MapAnimations animations;

/* Constructor  */  GuiMap(Clipboard & clipboard, const std::string & filePath);
                    GuiMap(Clipboard & clipboard, FileBrowserPtr<SaveType> fileBrowser = getDefaultOpenMapBrowser());
                    GuiMap(Clipboard & clipboard, Sc::Terrain::Tileset tileset, u16 width, u16 height, size_t terrainTypeIndex, DefaultTriggers defaultTriggers);

/*  Destructor  */  virtual ~GuiMap();

/*    File IO   */  bool CanExit(); // Returns true if there are no unsaved changes or user allows the exit
                    virtual bool SaveFile(bool saveAs);

/*   Chk Accel  */  bool setDoodadTile(size_t x, size_t y, u16 tileNum);
                    void setTileValue(size_t tileX, size_t tileY, uint16_t tileValue) final;
                    void setFogValue(size_t tileX, size_t tileY, u8 fogValue);
                    void beginTerrainOperation();
                    void finalizeTerrainOperation();
                    void finalizeFogOperation();
                    void validateTileOccupiers(size_t tileX, size_t tileY, uint16_t tileValue);
                    virtual void setTileset(Sc::Terrain::Tileset tileset);
                    void setDimensions(u16 newTileWidth, u16 newTileHeight, u16 sizeValidationFlags = SizeValidationFlag::Default, s32 leftEdge = 0, s32 topEdge = 0, size_t newTerrainType = 0);
                    bool placeIsomTerrain(Chk::IsomDiamond isomDiamond, size_t terrainType, size_t brushExtent);
                    void unlinkAndDeleteSelectedUnits();
                    void unlinkAndDeleteUnit(size_t unitIndex);
                    void changeUnitOwner(size_t unitIndex, u8 newPlayer);

/*   UI Accel   */  Layer getLayer();
                    bool setLayer(Layer newLayer);
                    TerrainSubLayer getSubLayer();
                    void setSubLayer(TerrainSubLayer newTerrainSubLayer);
                    double getZoom();
                    void setZoom(double newScale);
                    u8 getCurrPlayer() const;
                    bool setCurrPlayer(u8 newPlayer);
                    bool getCutCopyPasteTerrain() const;
                    void toggleCutCopyPasteTerrain();
                    bool getCutCopyPasteDoodads() const;
                    void toggleCutCopyPasteDoodads();
                    bool getCutCopyPasteSprites() const;
                    void toggleCutCopyPasteSprites();
                    bool getCutCopyPasteUnits() const;
                    void toggleCutCopyPasteUnits();
                    bool getCutCopyPasteFog() const;
                    void toggleCutCopyPasteFog();
                    bool isDragging();
                    void setDragging(bool dragging);

                    void convertSelectionToTerrain();
                    void stackSelected();
                    void createLocation();
                    void createInvertedLocation();
                    void createMobileInvertedLocation();

                    void viewUnit(u16 unitIndex);
                    void viewSprite(u16 spriteIndex);
                    void viewLocation(u16 locationId);
                    LocSelFlags getLocSelFlags(s32 xc, s32 yc);
                    bool moveLocation(u32 downX, u32 downY, u32 upX, u32 upY);

                    void doubleClickLocation(s32 xClick, s32 yClick);
                    void openTileProperties(s32 xClick, s32 yClick);
                    void EdgeDrag(HWND hWnd, int x, int y);

                    /** Takes a player number, outputs the string/string index of the associated owner (i.e. rescuable) */
                    u8 GetPlayerOwnerStringId(u8 player);

                    void refreshScenario(bool clearSelections = true);

/*   Sel/Paste  */  void clearSelectedTiles();
                    void clearSelectedDoodads();
                    void clearSelectedUnits();
                    void clearSelectedSprites();
                    void clearSelection();
                    void selectAll();
                    void deleteSelection();
                    void paste(s32 mapClickX, s32 mapClickY);
                    void PlayerChanged(u8 newPlayer);
                    u16 GetSelectedLocation();
                    bool autoSwappingAddonPlayers();
                    bool pastingToGrid();
                    void moveSelection(Direction direction, bool useGrid);

/*   Undo Redo  */  void undo();
                    void redo();
                    void checkUnsavedChanges();

/*   Graphics   */  float MiniMapScale(u16 xSize, u16 ySize);

                    bool EnsureBitmapSize(u32 desiredWidth, u32 desiredHeight);
                    void SnapSelEndDrag();
                    //void Animate(std::uint64_t currentTick);
                    bool UpdateGlGraphics();
                    void PaintMap(GuiMapPtr currMap, bool pasting);
                    void PaintMiniMap(const WinLib::DeviceContext & dc);
                    void Redraw(bool includeMiniMap);
                    void ValidateBorder(s32 screenWidth, s32 screenHeight, s32 newLeft = -1, s32 newTop = -1);

                    bool SetGridSize(s16 xSize, s16 ySize);
                    bool SetGridColor(u8 red, u8 green, u8 blue);

                    void ToggleDisplayBuildability();
                    bool DisplayingBuildability();
                    void ToggleDisplayElevations();
                    bool DisplayingElevations();

                    u32 getNextClassId();
                    bool isValidUnitPlacement(Sc::Unit::Type unitType, s32 x, s32 y);
                    bool isLinkable(const Chk::Unit & first, const Chk::Unit & second);
                    std::optional<u16> getLinkableUnitIndex(Sc::Unit::Type unitType, s32 x, s32 y);
                    
                    void ToggleBuildingsSnapToTile();
                    void ToggleUnitSnap();
                    void ToggleUnitStack();
                    void ToggleEnableAirStack();
                    void TogglePlaceUnitsAnywhere();
                    void TogglePlaceBuildingsAnywhere();
                    void ToggleAddonAutoPlayerSwap();
                    void ToggleRequireMineralDistance();

                    void ToggleDisplayIsomValues();
                    void ToggleTileNumSource(bool MTXMoverTILE);
                    bool DisplayingTileNums();

                    void ToggleDisplayFps();

                    void ToggleSpriteSnap();

                    void ToggleLocationNameClip();

                    enum class Snap : u32 { SnapToTile, SnapToGrid, NoSnap };
                    void SetLocationSnap(Snap locationSnap);

                    void ToggleLockAnywhere();
                    bool LockAnywhere();

                    void ToggleAllowIllegalDoodadPlacement();
                    bool AllowIllegalDoodadPlacement();

                    bool GetSnapIntervals(u32 & x, u32 & y, u32 & xOffset, u32 & yOffset);

                    enum_t(LocSnapFlags, u32, { SnapX1 = BIT_0, SnapY1 = BIT_1, SnapX2 = BIT_2, SnapY2 = BIT_3,
                        SnapAll = SnapX1|SnapY1|SnapX2|SnapY2, None = 0 });
                    bool SnapLocationDimensions(u32 & x1, u32 & y1, u32 & x2, u32 & y2, LocSnapFlags locSnapFlags);
                    bool SnapLocationDimensions(std::size_t locationIndex, LocSnapFlags locSnapFlags);
                    
                    void SetCutCopyPasteSnap(Snap cutCopyPasteSnap);
                    void ToggleIncludeDoodadTiles();
                    bool GetIncludeDoodadTiles();

                    void UpdateLocationMenuItems();
                    void UpdateDoodadMenuItems();
                    void UpdateZoomMenuItems();
                    void UpdateGridSizesMenu();
                    void UpdateGridColorMenu();
                    void UpdateSkinMenuItems();
                    void UpdateTerrainViewMenuItems();
                    void UpdateBaseViewMenuItems();
                    void UpdateUnitMenuItems();
                    void UpdateSpriteMenuItems();
                    void UpdateCutCopyPasteMenuItems();

                    void Scroll(bool scrollX, bool scrollY, bool validateBorder, s32 newLeft = -1, s32 newTop = -1);


/*     Misc     */  void setMapId(u16 mapId);
                    u16 getMapId();
                    void notifyUnsavedChanges(); // Adds an asterix onto the map name
                    void notifyNoUnsavedChanges(); // Removes an asterix from the map name
                    void updateMenu(); // Updates which items are checked in the main menu

                    bool CreateThis(HWND hClient, const std::string & title);
                    void ReturnKeyPress();
                    static void SetAutoBackup(bool doAutoBackups);
                    
                    void skipEventRendering();

                    ChkdPalette & getPalette();
                    ChkdPalette & getStaticPalette();

                    enum class Skin
                    {
                        ClassicGDI,
                        ClassicGL,
                        ScrSD,
                        ScrHD2,
                        ScrHD,
                        CarbotHD2,
                        CarbotHD
                    };
                    GuiMap::Skin GetSkin();
                    void SetSkin(GuiMap::Skin skin);

                    point getLastMousePosition() { return lastMousePosition; }


    protected:

                    LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
                    LRESULT MapMouseProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
                    LRESULT HorizontalScroll(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
                    LRESULT VerticalScroll(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
                    LRESULT DoSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
                    void ActivateMap(HWND deactivate, HWND activate);
                    LRESULT DestroyWindow(HWND hWnd);

                    void ContextMenu(int x, int y);
                    void RButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
                    void LButtonDoubleClick(int x, int y, WPARAM wParam);
                    void LButtonDown(int x, int y, WPARAM wParam);
                    void MouseMove(HWND hWnd, int x, int y, WPARAM wParam);
                    void MouseHover(HWND hWnd, int x, int y, WPARAM wParam);
                    void MouseWheel(HWND hWnd, int x, int y, int z, WPARAM wParam);
                    void LButtonUp(HWND hWnd, int x, int y, WPARAM wParam);
                    void MButtonDown(HWND hWnd, int x, int y, WPARAM wParam);
                    void MButtonUp(HWND hWnd, int x, int y, WPARAM wParam);
                    void PanTimerTimeout();
                    void FinalizeTerrainSelection(HWND hWnd, int mapX, int mapY, WPARAM wParam);
                    void FinalizeLocationDrag(HWND hWnd, int mapX, int mapY, WPARAM wParam);
                    void FinalizeUnitSelection(HWND hWnd, int mapX, int mapY, WPARAM wParam);
                    void FinalizeDoodadSelection(HWND hWnd, int mapX, int mapY, WPARAM wParam);
                    void FinalizeSpriteSelection(HWND hWnd, int mapX, int mapY, WPARAM wParam);
                    void FinalizeFogSelection(HWND hWnd, int mapX, int mapY, WPARAM wParam);
                    void FinalizeCutCopyPasteSelection(HWND hWnd, int mapX, int mapY, WPARAM wParam);
                    LRESULT ConfirmWindowClose(HWND hWnd);

                    bool GetBackupPath(time_t currTime, std::string & outFilePath);
                    bool TryBackup(bool & outCopyFailed);


    private:
                    void destroyBrush();
                    void refreshTileOccupationCache();
                    void windowBoundsChanged();

                    void tileSelectionsChanged();
                    void tileFogSelectionsChanged();
                    void checkSelChangeFlags();

                    void afterAction(std::size_t actionIndex) override;

/* Private Data */  Graphics scGraphics {*this, selections};
                    GuiMap::Skin skin = Skin::ClassicGDI;
                    std::shared_ptr<WinLib::DeviceContext> openGlDc;
                    u64 prevTickCount = GetTickCount64();
                    std::optional<RareEdit::Editor<Tracked>> brushAction {};
                    struct ScopedBrushDestructor { // Destroys the brush when this object goes out of scope
                        std::optional<RareEdit::Editor<Tracked>> & brushAction;
                        ~ScopedBrushDestructor() { brushAction = std::nullopt; }
                    };

                    u16 mapId = 0;
                    bool unsavedChanges = false;
                    std::size_t lastSaveActionCursor = 0;
                    std::size_t nonSelChangeCursor = std::numeric_limits<std::size_t>::max();

                    Layer currLayer = Layer::Terrain;
                    TerrainSubLayer currTerrainSubLayer = TerrainSubLayer::Isom;
                    Chk::TileOccupationCache tileOccupationCache;
                    u8 currPlayer = 0;
                    double zoom = 1.0;
					
                    bool dragging = false;

                    bool buildingsSnapToTile = true;
                    bool snapUnits = true;
                    bool stackUnits = false;
                    bool stackAirUnits = false;
                    bool placeUnitsAnywhere = false;
                    bool placeBuildingsAnywhere = false;
                    bool addonAutoPlayerSwap = true;
                    bool requireMineralDistance = true;

                    bool snapLocations = true;
                    bool locSnapTileOverGrid = true;
                    bool lockAnywhere = true;
                    bool allowIllegalDoodads = false;

                    bool snapSprites = true;

                    bool snapCutCopyPasteSel = true;
                    bool cutCopyPasteSnapTileOverGrid = false;
                    bool cutCopyPasteIncludeDoodadTiles = true;

                    bool cutCopyPasteTerrain = true;
                    bool cutCopyPasteDoodads = true;
                    bool cutCopyPasteSprites = true;
                    bool cutCopyPasteUnits = true;
                    bool cutCopyPasteFog = false;

                    bool skipEventRender = false;
					
                    UINT_PTR panTimerID = 0;
                    int panStartX = -1;
                    int panStartY = -1;
                    int panCurrentX = 0;
                    int panCurrentY = 0;

                    ChkdBitmap graphicBits {};
                    point lastMousePosition {};
                    s32 screenLeft = 0;
                    s32 screenTop = 0;
                    u32 bitmapWidth = 0;
                    u32 bitmapHeight = 0;
                    bool RedrawMiniMap = true;
                    bool RedrawMap = true;
                    WinLib::DeviceContext miniMapBuffer {};
                    WinLib::DeviceContext mapBuffer {};
                    WinLib::DeviceContext toolsBuffer {};

                    static bool doAutoBackups;
                    double minSecondsBetweenBackups = 1800; // The smallest interval between consecutive backups
                    time_t lastBackupTime = -1; // -1 if there are no previous backups
                    std::chrono::system_clock::time_point lastMoveEdgeDrag = std::chrono::system_clock::now();

                    GuiMap();
};

#endif