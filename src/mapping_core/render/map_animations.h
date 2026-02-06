#ifndef MAPANIMATIONS_H
#define MAPANIMATIONS_H
#include "../sc.h"
#include "../scenario.h"
#include "game_clock.h"
#include "map_actor.h"
#include "map_image.h"
#include <optional>

struct AnimContext
{
protected:
    std::vector<u16> availableImages {};

    u16 createImage();
    void removeImage(u16 imageIndex);
    void initializeImage(u32 iScriptId, std::uint64_t currentTick, MapActor & actor, bool isUnit, std::size_t imageIndex);

public:
    const Sc::Data & scData;
    const GameClock & gameClock;

    AnimContext(const Sc::Data & scData, const GameClock & gameClock) : scData(scData), gameClock(gameClock) {}

    static constexpr std::uint64_t UnusedDrawEntry  = 0xFFFFFFFFFFFFFFFFull;
    static constexpr std::uint64_t MaskIndex        =         0xFFFFFFFFull;
    static constexpr std::uint64_t FlagDrawUnit     =        0x100000000ull;
    static constexpr std::uint64_t FlagUnitActor    =        0x200000000ull;
    static constexpr std::uint64_t FlagIsTurret     =        0x400000000ull;
    static constexpr std::uint64_t MaskY            =     0xFFF800000000ull;
    static constexpr std::uint64_t MaskElevation    =   0xFF000000000000ull;
    static constexpr std::uint64_t ShiftDrawUnit    = 32;
    static constexpr std::uint64_t ShiftUnitActor   = 33;
    static constexpr std::uint64_t ShiftIsTurret    = 34;
    static constexpr std::uint64_t ShiftY           = 35;
    static constexpr std::uint64_t ShiftElevation   = 48;
    static constexpr std::uint64_t MaskNonIndexDrawList = 0xFFFFFFFF00000000;
    // unitOrSpriteIndex | drawUnit << 32 | unitActor << 33 | isTurret << 34 | y << 35 | elevation << 48

    bool drawListDirty = true; // If true at the end of an anim tick, sort the draw list, then go through all entries to give the new drawListIndexes to actors
    std::vector<std::uint64_t> drawList { 0ull }; // Index 0 of drawList is unused, a value of 0xFFFFFFFF in a drawList entry indicates an unused index
    std::vector<std::optional<MapImage>> images {MapImage{}}; // contains the images associated with unitActors and spriteActors, image 0 is unused
    
    void clearActor(MapActor & actor);
    void restartActor(ActorContext & context);
    void initializeActor(MapActor & actor, u8 direction, u16 imageId, u8 owner, s32 xc, s32 yc, u32 iScriptId, bool isUnit, bool autoRestart, std::uint64_t drawListValue);
    void initSpecialCases(MapActor & actor, std::size_t type, bool isUnit, bool isSpriteUnit = false);
    void initializeUnitActor(MapActor & actor, bool isClipboard, std::size_t unitIndex, const Chk::Unit & unit, s32 xc, s32 yc);
    void initializeSpriteActor(MapActor & actor, bool isClipboard, std::size_t spriteIndex, const Chk::Sprite & sprite, s32 xc, s32 yc);
    
    friend struct MapActor;
    friend struct MapImage;
    friend struct Animator;
};

class MapAnimations : public AnimContext
{
protected:
    const Scenario & scenario;

public:
    MapAnimations(const Sc::Data & scData, const GameClock & gameClock, const Scenario & scenario);

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
    void updateUnitResourceAmount(std::size_t unitIndex, u32 oldResourceAmount, u32 newResourceAmount);
    void updateUnitStateFlags(std::size_t unitIndex, u16 oldStateFlags, u16 newStateFlags);
    void updateUnitRelationFlags(std::size_t unitIndex, u16 oldRelationFlags, u16 newRelationFlags);
    void updateSpriteFlags(std::size_t spriteIndex, u16 oldSpriteFlags, u16 newSpriteFlags);

    void cleanDrawList();
    void animate(uint64_t currentTick);
};

#endif