#ifndef CLIPBOARD_H
#define CLIPBOARD_H
#include <common_files/constants.h>
#include <common_files/structs.h>
#include <mapping_core/map_actor.h>
#include <mapping_core/mapping_core.h>
#include <chrono>
#include <string>

class GuiMap;

using TileNeighbor = Sc::Terrain::TileNeighbor;

void StringToWindowsClipboard(const std::string & str);

bool WindowsClipboardToString(std::string & str);

struct PasteTileNode
{
    u16 value = 0;
    s32 xc = 0;
    s32 yc = 0;
    TileNeighbor neighbors = TileNeighbor::None; // Same as in TileNode

    PasteTileNode() = delete;
    PasteTileNode(u16 value, s32 xc, s32 yc, TileNeighbor neighbors) :
        value(value), xc(xc), yc(yc), neighbors(neighbors) {}
};

class PasteDoodadNode
{
    const Sc::Terrain::DoodadPlacibility* doodadPlacibility;

public:
    u16 doodadId = 0;
    u16 tileIndex[16][16] {};
    u16 tileWidth = 0;
    u16 tileHeight = 0;
    s32 tileX = 0;
    s32 tileY = 0;
    u8 owner = 0;
    u16 overlayIndex = 0;
    u16 spriteFlags = 0;

    constexpr bool isSprite() const { return (spriteFlags & Chk::Sprite::SpriteFlags::DrawAsSprite) == Chk::Sprite::SpriteFlags::DrawAsSprite; }
    constexpr bool overlayFlipped() const { return (spriteFlags & Chk::Sprite::SpriteFlags::OverlayFlipped_Deprecated) == Chk::Sprite::SpriteFlags::OverlayFlipped_Deprecated; }

    PasteDoodadNode() = delete;
    PasteDoodadNode(u16 startTileGroup, s32 tileX, s32 tileY);
    PasteDoodadNode(const Chk::Doodad & doodad);

    bool isPlaceable(const Scenario & scenario, s32 xTileStart, s32 yTileStart) const;
};

struct PasteSpriteNode
{
    Chk::Sprite sprite {};
    s32 xc = 0;
    s32 yc = 0;

    PasteSpriteNode() = delete;
    PasteSpriteNode(const Chk::Sprite & sprite);
};

struct PasteUnitNode
{
    Chk::Unit unit {};
    s32 xc = 0;
    s32 yc = 0;

    PasteUnitNode() = delete;
    PasteUnitNode(const Chk::Unit & unit) : unit(unit), xc(unit.xc), yc(unit.yc) {}
};

struct PasteFogTileNode
{
    u8 value = 0;
    s32 xc = 0;
    s32 yc = 0;
    TileNeighbor neighbors = TileNeighbor::None; // Same as in FogTile

    PasteFogTileNode() = delete;
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
    bool pasting = false;
    bool quickPaste = false; /* Controls whether you are using quick placement,
                                as opposed to something saved on clipboard.     */
    bool fillSimilarTiles = false;
    rect edges { -1, -1, -1, -1 };
    size_t terrainTypeIndex = 0;
    Chk::IsomDiamond prevIsomPaste = Chk::IsomDiamond::none();
    std::vector<PasteTileNode> copyTiles {};
    std::vector<PasteTileNode> quickTiles {};
    std::vector<PasteDoodadNode> copyDoodads {};
    std::vector<PasteDoodadNode> quickDoodads {};
    std::vector<PasteSpriteNode> copySprites {};
    std::vector<PasteSpriteNode> quickSprites {};
    std::vector<PasteUnitNode> copyUnits {};
    std::vector<PasteUnitNode> quickUnits {};
    std::vector<PasteFogTileNode> copyFogTiles {};
    FogBrush fogBrush {};
    std::chrono::time_point<std::chrono::steady_clock> lastPasteTime = std::chrono::steady_clock::now();
    std::optional<u16> lastPasteNydus = std::nullopt;

    point prevPaste { -1, -1 };

    bool isNearPrevPaste(s32 mapClickX, s32 mapClickY);

    void ClearQuickItems();

    void pasteTerrain(TerrainSubLayer terrainSubLayer, s32 mapClickX, s32 mapClickY, GuiMap & map, point prevPaste);
    void pasteDoodads(s32 mapClickX, s32 mapClickY, GuiMap & map, point prevPaste);
    void fillPasteTerrain(s32 mapClickX, s32 mapClickY, GuiMap & map, point prevPaste);
    void pasteUnits(s32 mapClickX, s32 mapClickY, GuiMap & map, bool allowStack, point prevPaste);
    void pasteSprites(s32 mapClickX, s32 mapClickY, GuiMap & map, point prevPaste);
    void pasteBrushFog(s32 mapClickX, s32 mapClickY, GuiMap & map, point prevPaste);
    void pasteFog(s32 mapClickX, s32 mapClickY, GuiMap & map, point prevPaste);
        
    void updateTileMiddle(point middle);
    void updateDoodadMiddle(point middle);
    void updateUnitMiddle(point middle);
    void updateSpriteMiddle(point middle);
    void updateFogMiddle(point middle);

public:
    std::vector<MapActor> unitActors;
    std::vector<MapActor> spriteActors;

    bool hasTiles() const;
    bool hasDoodads() const;
    bool hasUnits() const { return copyUnits.size() > 0; }
    bool hasSprites() const { return copySprites.size() > 0; }
    void copy(GuiMap & map, Layer layer);

    void setQuickIsom(size_t terrainTypeIndex);
    void setQuickDoodad(u16 doodadStartTileGroup);
        
    void setQuickTile(u16 index, s32 xc, s32 yc);
    void addQuickTile(u16 index, s32 xc, s32 yc);
    bool hasQuickTiles() const { return quickTiles.size() > 0; }

    void addQuickUnit(const Chk::Unit & unit);
    bool hasQuickUnits() const { return quickUnits.size() > 0; }
        
    void addQuickSprite(const Chk::Sprite & sprite);
    bool hasQuickSprites() const { return quickSprites.size() > 0; }

    bool hasFogTiles() const { return copyFogTiles.size() > 0; }

    const auto & getFogBrush() const { return this->fogBrush; }
    void setFogBrushSize(u32 width, u32 height);
    void initFogBrush(s32 mapClickX, s32 mapClickY, const GuiMap & map, bool allPlayers);

    void beginPasting(bool isQuickPaste, GuiMap & map);
    void endPasting(GuiMap* map);

    void doPaste(Layer layer, TerrainSubLayer terrainSubLayer, s32 mapClickX, s32 mapClickY, GuiMap & map, bool allowStack);
        
    bool getFillSimilarTiles() const;
    void toggleFillSimilarTiles();
    
    std::vector<PasteTileNode> & getTiles();
    std::vector<PasteDoodadNode> & getDoodads();
    std::vector<PasteUnitNode> & getUnits();
    std::vector<PasteSpriteNode> & getSprites();
    std::vector<PasteFogTileNode> & getFogTiles();
    const std::vector<PasteTileNode> & getTiles() const;
    const std::vector<PasteDoodadNode> & getDoodads() const;
    const std::vector<PasteUnitNode> & getUnits() const;
    const std::vector<PasteSpriteNode> & getSprites() const;
    const std::vector<PasteFogTileNode> & getFogTiles() const;
    bool isPasting() { return pasting; }
    bool isQuickPasting() { return pasting && quickPaste; }
    bool isPreviousPasteLoc(u16 x, u16 y) { return x == prevPaste.x && y == prevPaste.y; }
    void clearPreviousPasteLoc() { prevIsomPaste = Chk::IsomDiamond::none(); }
};

#endif