#ifndef MAPS_H
#define MAPS_H
#include "../../CommonFiles/CommonFiles.h"
#include "../../Mapping/WglRenderContext.h"
#include "GuiMap.h"
#include <map>
#include <exception>

class Maps : public WinLib::MdiClient
{
    public:
        GuiMapPtr currentlyActiveMap;

        Clipboard clipboard;

        Maps();
        virtual ~Maps();

        bool isInOpenMaps(GuiMapPtr guiMap);
        bool Focus(HWND hGuiMap);
        bool Focus(GuiMapPtr guiMap);
        GuiMapPtr GetMap(HWND hGuiMap);
        GuiMapPtr GetMap(u16 mapId);
        u16 GetMapID(GuiMapPtr guiMap);

        GuiMapPtr NewMap(Sc::Terrain::Tileset tileset = Sc::Terrain::Tileset::Badlands,
                         u16 width = 64,
                         u16 height = 64,
                         size_t terrainTypeIndex = Sc::Isom::defaultBrushIndex[Sc::Terrain::Tileset::Badlands],
                         DefaultTriggers defaultTriggers = DefaultTriggers::NoTriggers);
        bool OpenMap(const std::string & fileName);
        bool OpenMap(FileBrowserPtr<SaveType> fileBrowser = MapFile::getDefaultOpenMapBrowser());
        bool SaveCurr(bool saveAs);
        void CloseMap(HWND hMap);
        bool CloseAll();
        void CloseActive();

        void UpdateTreeView();
        void SetGrid(s16 xSize, s16 ySize);
        void SetGridColor(u8 red, u8 green, u8 blue);
        void ChangeLayer(Layer newLayer);
        void ChangeSubLayer(TerrainSubLayer subLayer);
        void ChangeZoom(bool increment);
        void ChangePlayer(u8 newPlayer, bool updateMapPlayers = true);
        void UpdatePlayerStatus();
        bool toggleCutCopyPasteTerrain();
        bool toggleCutCopyPasteDoodads();
        bool toggleCutCopyPasteSprites();
        bool toggleCutCopyPasteUnits();
        bool toggleCutCopyPasteFog();
        void cut();
        void copy();
        void startPaste(bool isQuickPaste);
        void endPaste();
        void properties();

        void stickCursor(); // Ensures that the cursor does revert during click & drags
        void updateCursor(s32 xc, s32 yc);

        // Creates the OpenGL rendering context with the given context if it doesn't exist or associates it with this new device context if it does
        void createRenderContext(std::shared_ptr<WinLib::DeviceContext> deviceContext);

    protected:
        u16 NextId();
        void EnableMapping();
        void DisableMapping();

        void AddMap(GuiMapPtr map);
        bool RemoveMap(GuiMapPtr guiMap);

    private:
        bool mappingEnabled;
        std::multimap<u16, GuiMapPtr> openMaps; // <mapId, map>
        std::optional<WglRenderContext> openGlContext; // Context is created by the first map to need it, owned by the currently active map (if any)
        int UntitledNumber;
        u16 lastUsedMapID;

        bool nonStandardCursor;
        HCURSOR* currCursor;
        HCURSOR standardCursor;
        HCURSOR sizeAllCursor;
        HCURSOR neswCursor;
        HCURSOR nwseCursor;
        HCURSOR nsCursor;
        HCURSOR weCursor;
};

class AllMapIdsExausted : public std::exception
{
public:
    virtual const char* what() { return "All 65535 map ids are in use, please close some maps or restart Chkdraft to open additional maps!"; }
};

#endif