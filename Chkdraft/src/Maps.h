#ifndef MAPS_H
#define MAPS_H
#include "CommonFiles/CommonFiles.h"
#include "GuiMap.h"
#include <map>

class Maps : public MdiClient
{
    public:
        GuiMapPtr currentlyActiveMap;

        Clipboard clipboard;

        Maps();
        ~Maps();

        bool isInOpenMaps(GuiMapPtr guiMap);
        bool Focus(HWND hGuiMap);
        bool Focus(GuiMapPtr guiMap);
        GuiMapPtr GetMap(HWND hGuiMap);
        GuiMapPtr GetMap(u16 mapId);
        u16 GetMapID(GuiMapPtr guiMap);

        bool NewMap(u16 width, u16 height, u16 tileset, u32 terrain, u32 triggers);
        bool OpenMap(std::string fileName);
        bool OpenMap(); // Accelerator for OpenMap
        bool SaveCurr(bool saveAs);
        void CloseMap(HWND hMap);
        void CloseActive();

        void UpdateTreeView();
        void SetGrid(s16 xSize, s16 ySize);
        void SetGridColor(u8 red, u8 green, u8 blue);
        void ChangeLayer(Layer newLayer);
        void ChangeZoom(bool increment);
        void ChangePlayer(u8 newPlayer);
        void cut();
        void copy();
        void startPaste(bool isQuickPaste);
        void endPaste();
        void properties();

        void stickCursor(); // Ensures that the cursor does revert during click & drags
        void updateCursor(s32 xc, s32 yc);

    protected:
        u16 NextId();
        void EnableMapping();
        void DisableMapping();

        GuiMapPtr AddEmptyMap();
        bool RemoveMap(GuiMapPtr guiMap);

    private:
        bool mappingEnabled;
        std::multimap<u16, GuiMapPtr> openMaps; // <mapId, map>
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

#endif