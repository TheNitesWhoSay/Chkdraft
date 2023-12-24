#ifndef CLIPBOARD_H
#define CLIPBOARD_H
#include "../CommonFiles/CommonFiles.h"
#include "../../MappingCoreLib/MappingCore.h"
#include "Selections.h"
#include "Undos/Undos.h"
#include <chrono>
class GuiMap;

void StringToWindowsClipboard(const std::string & str);

bool WindowsClipboardToString(std::string & str);

class PasteTileNode
{
    public:
        u16 value;
        s32 xc;
        s32 yc;
        TileNeighbor neighbors; // Same as in TileNode

        PasteTileNode(u16 value, s32 xc, s32 yc, TileNeighbor neighbors) :
            value(value), xc(xc), yc(yc), neighbors(neighbors) {}
        virtual ~PasteTileNode();
};

class PasteDoodadNode
{
    public:
        u16 doodadId;
        u16 tileIndex[16][16] {};
        u16 tileWidth = 0;
        u16 tileHeight = 0;
        s32 tileX = 0;
        s32 tileY = 0;
        u8 owner = 0;
        u16 overlayIndex = 0;
        u16 spriteFlags = 0;
        const Sc::Terrain::DoodadPlacibility* doodadPlacibility;
        
        constexpr bool isSprite() const { return (spriteFlags & Chk::Sprite::SpriteFlags::DrawAsSprite) == Chk::Sprite::SpriteFlags::DrawAsSprite; }
        constexpr bool overlayFlipped() const { return (spriteFlags & Chk::Sprite::SpriteFlags::OverlayFlipped_Deprecated) == Chk::Sprite::SpriteFlags::OverlayFlipped_Deprecated; }
        
        PasteDoodadNode(u16 startTileGroup, s32 tileX, s32 tileY);
        PasteDoodadNode(const Chk::Doodad & doodad);
        virtual ~PasteDoodadNode();

        bool isPlaceable(const Scenario & scenario, s32 xTileStart, s32 yTileStart) const;
};

class PasteSpriteNode
{
    public:
        Chk::Sprite sprite;
        s32 xc;
        s32 yc;
        
        PasteSpriteNode() = delete;
        PasteSpriteNode(const Chk::Sprite & sprite);
        virtual ~PasteSpriteNode();
};

class PasteUnitNode
{
    public:
        Chk::Unit unit;
        s32 xc;
        s32 yc;

        PasteUnitNode(const Chk::Unit & unit);
        virtual ~PasteUnitNode();

    private:
        PasteUnitNode(); // Disallow ctor
};

struct PasteFogTileNode
{
    u8 value;
    s32 xc;
    s32 yc;
    TileNeighbor neighbors; // Same as in FogTile
    
    PasteFogTileNode() = delete; // Disallow ctor
    PasteFogTileNode(u8 value, s32 xc, s32 yc, TileNeighbor neighbors) :
        value(value), xc(xc), yc(yc), neighbors(neighbors) {}
};

struct FogBrush
{
    u32 width = 1;
    u32 height = 1;
    bool setFog = false; // If false, clearing fog
    bool allPlayers = false;
};

class Clipboard
{
    public:

        Clipboard();
        virtual ~Clipboard();

        bool hasTiles();
        bool hasDoodads();
        bool hasUnits() { return copyUnits.size() > 0; }
        bool hasSprites() { return copySprites.size() > 0; }
        void copy(GuiMap & map, Layer layer);

        void setQuickIsom(size_t terrainTypeIndex);
        void setQuickDoodad(u16 doodadStartTileGroup);
        
        void addQuickTile(u16 index, s32 xc, s32 yc);
        bool hasQuickTiles() { return quickTiles.size() > 0; }

        void addQuickUnit(const Chk::Unit & unit);
        bool hasQuickUnits() { return quickUnits.size() > 0; }
        
        void addQuickSprite(const Chk::Sprite & sprite);
        bool hasQuickSprites() { return quickSprites.size() > 0; }

        bool hasFogTiles() { return copyFogTiles.size() > 0; }

        const auto & getFogBrush() { return this->fogBrush; }
        void setFogBrushSize(u32 width, u32 height);
        void initFogBrush(s32 mapClickX, s32 mapClickY, const GuiMap & map, bool allPlayers);

        void beginPasting(bool isQuickPaste);
        void endPasting();

        void doPaste(Layer layer, TerrainSubLayer terrainSubLayer, s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos, bool allowStack);
        
        bool getFillSimilarTiles();
        void toggleFillSimilarTiles();

        std::vector<PasteTileNode> & getTiles();
        std::vector<PasteDoodadNode> & getDoodads();
        std::vector<PasteUnitNode> & getUnits();
        std::vector<PasteSpriteNode> & getSprites();
        std::vector<PasteFogTileNode> & getFogTiles();
        bool isPasting() { return pasting; }
        bool isQuickPasting() { return pasting && quickPaste; }
        bool isPreviousPasteLoc(u16 x, u16 y) { return x == prevPaste.x && y == prevPaste.y; }
        void clearPreviousPasteLoc() { prevIsomPaste = Chk::IsomDiamond::none(); }


    protected:

        void pasteTerrain(TerrainSubLayer terrainSubLayer, s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos);
        void pasteDoodads(s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos);
        void fillPasteTerrain(s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos);
        void pasteUnits(s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos, bool allowStack);
        void pasteSprites(s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos);
        void pasteBrushFog(s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos);
        void pasteFog(s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos);
        
        void updateTileMiddle(POINT middle);
        void updateDoodadMiddle(POINT middle);
        void updateUnitMiddle(POINT middle);
        void updateSpriteMiddle(POINT middle);
        void updateFogMiddle(POINT middle);


    private:

        bool pasting;
        bool quickPaste; /* Controls whether you are using quick placement,
                            as opposed to something saved on clipboard.     */
        bool fillSimilarTiles;
        RECT edges;
        size_t terrainTypeIndex = 0;
        Chk::IsomDiamond prevIsomPaste = Chk::IsomDiamond::none();
        std::vector<PasteTileNode> copyTiles;
        std::vector<PasteTileNode> quickTiles;
        std::vector<PasteDoodadNode> copyDoodads;
        std::vector<PasteDoodadNode> quickDoodads;
        std::vector<PasteSpriteNode> copySprites;
        std::vector<PasteSpriteNode> quickSprites;
        std::vector<PasteUnitNode> copyUnits;
        std::vector<PasteUnitNode> quickUnits;
        std::vector<PasteFogTileNode> copyFogTiles;
        FogBrush fogBrush {};
        std::chrono::time_point<std::chrono::steady_clock> lastPasteTime = std::chrono::steady_clock::now();

        POINT prevPaste;

        bool isNearPrevPaste(s32 mapClickX, s32 mapClickY);

        void ClearQuickItems();
};

#endif