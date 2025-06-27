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

    static constexpr std::uint64_t MaskNonIndexDrawList = 0xFFFFFFFF00000000;
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
    void clearActor(MapActor & actor);
    void restartActor(AnimationContext & context);
    void initializeActor(MapActor & actor, u8 direction, u16 imageId, u8 owner, s32 xc, s32 yc, u32 iScriptId, bool isUnit, bool autoRestart, std::uint64_t drawListValue);
    void initSpecialCases(MapActor & actor, std::size_t type, bool isUnit, bool isSpriteUnit = false);
    void initializeUnitActor(MapActor & actor, bool isClipboard, std::size_t unitIndex, const Chk::Unit & unit, s32 xc, s32 yc);
    void initializeSpriteActor(MapActor & actor, bool isClipboard, std::size_t spriteIndex, const Chk::Sprite & sprite, s32 xc, s32 yc);
    void addUnit(std::size_t unitIndex, MapActor & actor);
    void addSprite(std::size_t spriteIndex, MapActor & actor);
    void removeUnit(std::size_t unitIndex, MapActor & actor);
    void removeSprite(std::size_t spriteIndex, MapActor & actor);
    void updateUnitType(std::size_t unitIndex, Sc::Unit::Type newUnitType);
    void updateSpriteType(std::size_t spriteIndex, Sc::Sprite::Type newSpriteType);
    void updateUnitOwner(std::size_t unitIndex, u8 newUnitOwner);
    void updateSpriteOwner(std::size_t spriteIndex, u8 newSpriteOwner);
    void updateUnitIndex(std::size_t unitIndexFrom, std::size_t unitIndexTo);
    void updateSpriteIndex(std::size_t spriteIndexFrom, std::size_t spriteIndexTo);
    void updateUnitXc(std::size_t unitIndex, u16 oldXc, u16 newXc);
    void updateUnitYc(std::size_t unitIndex, u16 oldYc, u16 newYc);
    void updateSpriteXc(std::size_t spriteIndex, u16 oldXc, u16 newXc);
    void updateSpriteYc(std::size_t spriteIndex, u16 oldYc, u16 newYc);
    void updateUnitStateFlags(std::size_t unitIndex, u16 oldStateFlags, u16 newStateFlags);
    void updateSpriteFlags(std::size_t spriteIndex, u16 oldSpriteFlags, u16 newSpriteFlags);
    void cleanDrawList();

    void animate(uint64_t currentTick);
    
    friend struct MapActor;
    friend struct MapImage;
    friend struct Animator;
};

#endif