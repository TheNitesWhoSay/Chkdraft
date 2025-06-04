#ifndef CLIPBOARD_H
#define CLIPBOARD_H
#include <common_files/constants.h>
#include <common_files/structs.h>
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
    PasteSpriteNode(const Chk::Sprite & sprite) : sprite(sprite), xc(sprite.xc), yc(sprite.yc) {}
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
    bool hasTiles();
    bool hasDoodads();
    bool hasUnits() { return copyUnits.size() > 0; }
    bool hasSprites() { return copySprites.size() > 0; }
    void copy(GuiMap & map, Layer layer);

    void setQuickIsom(size_t terrainTypeIndex);
    void setQuickDoodad(u16 doodadStartTileGroup);
        
    void setQuickTile(u16 index, s32 xc, s32 yc);
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

    void doPaste(Layer layer, TerrainSubLayer terrainSubLayer, s32 mapClickX, s32 mapClickY, GuiMap & map, bool allowStack);
        
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
};

#endif