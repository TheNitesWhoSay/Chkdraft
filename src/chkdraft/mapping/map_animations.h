#ifndef MAPANIMATIONS_H
#define MAPANIMATIONS_H
#include "mapping_core/map_actor.h"
#include "mapping_core/map_image.h"
#include "mapping_core/mapping_core.h"
#include <optional>

class Clipboard;
struct PasteUnitNode;
struct PasteSpriteNode;

class MapAnimations
{
protected:
    const Scenario & scenario;
    Clipboard & clipboard;
    std::vector<u16> availableImages {};

    u16 createImage();
    void removeImage(u16 imageIndex);
    void initializeImage(u32 iScriptId, std::uint64_t currentTick, MapActor & actor, bool isUnit, std::size_t imageIndex);

public:
    static constexpr std::uint64_t UnusedDrawEntry  = 0xFFFFFFFFFFFFFFFFull;
    static constexpr std::uint64_t MaskIndex        =         0xFFFFFFFFull;
    static constexpr std::uint64_t FlagDrawUnit     =        0x100000000ull;
    static constexpr std::uint64_t FlagUnitActor    =        0x200000000ull;
    static constexpr std::uint64_t FlagIsTurret     =        0x400000000ull;
    static constexpr std::uint64_t MaskY            =     0xFFF800000000ull;
    static constexpr std::uint64_t MaskElevation    =   0x1F000000000000ull;
    static constexpr std::uint64_t FlagIsClipboard  = 0x8000000000000000ull;
    static constexpr std::uint64_t ShiftDrawUnit    = 32;
    static constexpr std::uint64_t ShiftUnitActor   = 33;
    static constexpr std::uint64_t ShiftIsTurret    = 34;
    static constexpr std::uint64_t ShiftY           = 35;
    static constexpr std::uint64_t ShiftElevation   = 48;

    // unitOrSpriteIndex | isUnit << 32 | isTurret << 33 | y << 34 | elevation << 47
    bool drawListDirty = true; // If true at the end of an anim tick, sort the draw list, then go through all entries to give the new drawListIndexes to actors
    std::vector<std::uint64_t> drawList { 0ull }; // Index 0 of drawList is unused, a value of 0xFFFFFFFF in a drawList entry indicates an unused index
    std::vector<std::optional<MapImage>> images; // contains the images associated with unitActors and spriteActors

    MapAnimations(const Scenario & scenario, Clipboard & clipboard);

    void initClipboardUnits();
    void initClipboardSprites();
    void clearClipboardUnits();
    void clearClipboardSprites();
    void clearClipboardActors();
    void addUnit(std::size_t unitIndex, MapActor & actor);
    void addSprite(std::size_t spriteIndex, MapActor & actor);
    void removeUnit(std::size_t unitIndex, MapActor & actor);
    void removeSprite(std::size_t spriteIndex, MapActor & actor);
    void cleanDrawList();

    void animate(uint64_t currentTick);
    
    friend struct MapActor;
    friend struct MapImage;
    friend struct Animator;
};

#endif