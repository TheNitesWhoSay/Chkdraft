#include "map_animations.h"
#include <cross_cut/logger.h>

extern Logger logger;

u32 iscriptIdFromUnit(const Sc::Data & scData, Sc::Unit::Type unitType)
{
    return scData.sprites.getImage(
        scData.sprites.getSprite(
            scData.units.getFlingy(
                scData.units.getUnit(
                    size_t(unitType) < scData.units.numUnitTypes() ? unitType : Sc::Unit::Type(0)
                ).graphics
            ).sprite
        ).imageFile
    ).iScriptId;
}

u32 iscriptIdFromSprite(const Sc::Data & scData, Sc::Sprite::Type spriteType)
{
    return scData.sprites.getImage(
        scData.sprites.getSprite(
            size_t(spriteType) < scData.sprites.numSprites() ? spriteType : Sc::Sprite::Type(0)
        ).imageFile
    ).iScriptId;
}

size_t getImageId(const Sc::Data & scData, Sc::Unit::Type unitType)
{
    u32 flingyId = u32(scData.units.getUnit(Sc::Unit::Type(unitType >= scData.units.numUnitTypes() ? 0 : unitType)).graphics);
    u32 spriteId = u32(scData.units.getFlingy(flingyId >= scData.units.numFlingies() ? 0 : flingyId).sprite);
    return scData.sprites.getSprite(spriteId >= scData.sprites.numSprites() ? 0 : spriteId).imageFile;
}

size_t getImageId(const Sc::Data & scData, Sc::Sprite::Type spriteType)
{
    return scData.sprites.getSprite(spriteType >= scData.sprites.numSprites() ? 0 : spriteType).imageFile;
}

size_t getImageId(const Sc::Data & scData, const Chk::Unit & unit)
{
    return getImageId(scData, unit.type);
}

size_t getImageId(const Sc::Data & scData, Sc::Sprite::Type spriteType, bool isDrawnAsSprite)
{
    return isDrawnAsSprite ?
        getImageId(scData, spriteType) :
        getImageId(scData, Sc::Unit::Type(spriteType));
}

size_t getImageId(const Sc::Data & scData, const Chk::Sprite & sprite)
{
    return sprite.isDrawnAsSprite() ?
        getImageId(scData, sprite.type) :
        getImageId(scData, Sc::Unit::Type(sprite.type));
}

struct ResourceLevel
{
    enum type {
        Low, // [,250)
        Med, // [250,500)
        MedHigh, // [500,750)
        High // [750,)
    };

    static constexpr auto get(u32 resourceAmount)
    {
        if ( resourceAmount >= 750 )
            return ResourceLevel::High; // [750,)
        else if ( resourceAmount >= 500 )
            return ResourceLevel::MedHigh; // [500,750)
        else if ( resourceAmount >= 250 )
            return ResourceLevel::Med; // [250,500)
        else
            return ResourceLevel::Low; // [,250)
    }

    static constexpr auto animHeader(ResourceLevel::type resourceLevel)
    {
        switch ( resourceLevel )
        {
            case ResourceLevel::Low: return Sc::Sprite::AnimHeader::SpecialState1;
            case ResourceLevel::Med: return Sc::Sprite::AnimHeader::SpecialState2;
            case ResourceLevel::MedHigh: return Sc::Sprite::AnimHeader::AlmostBuilt;
            case ResourceLevel::High: default: return Sc::Sprite::AnimHeader::WorkingToIdle;
        }
    }
};

u16 AnimContext::createImage()
{
    if ( availableImages.empty() )
    {
        images.emplace_back(MapImage{});
        return static_cast<u16>(images.size()-1);
    }
    else
    {
        auto imageIndex = availableImages.back();
        availableImages.pop_back();
        images[static_cast<std::size_t>(imageIndex)] = MapImage{};
        return imageIndex;
    }
}

void AnimContext::removeImage(u16 imageIndex)
{
    if ( imageIndex == 0 )
        return;

    images[imageIndex] = std::nullopt;
    availableImages.push_back(imageIndex);
}

void AnimContext::initializeImage(u32 iScriptId, std::uint64_t currentTick, MapActor & actor, bool isUnit, std::size_t imageIndex)
{
    ActorContext context {
        .currentTick = currentTick,
        .animations = *this,
        .actor = actor,
        .isUnit = isUnit
    };
    Animator {
        .context = context,
        .currImageIndex = imageIndex,
        .currImage = &images[imageIndex].value()
    }.initializeImage(iScriptId);
}

void AnimContext::clearActor(MapActor & actor)
{
    if ( actor.drawListIndex > 0 && actor.drawListIndex < drawList.size() )
        drawList[actor.drawListIndex] = UnusedDrawEntry;

    for ( auto usedImageIndex : actor.usedImages )
        removeImage(usedImageIndex);

    actor = {};
}

void AnimContext::restartActor(ActorContext & context)
{
    auto & actor = context.actor;
    actor.noBreakSection = false;
    for ( std::size_t i=0; i<std::size(actor.usedImages); ++i ) // Remove all non-primary actors
    {
        if ( i != actor.primaryImageSlot )
        {
            removeImage(actor.usedImages[i]);
            actor.usedImages[i] = 0;
        }
    }
    if ( actor.primaryImageSlot < std::size(actor.usedImages) )
    {
        if ( actor.primaryImageSlot != 0 )
        {
            std::swap(actor.usedImages[0], actor.usedImages[actor.primaryImageSlot]);
            actor.primaryImageSlot = 0;
        }
        std::size_t imageIndex = static_cast<std::size_t>(actor.usedImages[0]);
        if ( imageIndex >= images.size() || images[imageIndex] == std::nullopt )
        {
            logger.error("While restarting a sprite an invalid primary image was found!");
            actor.autoRestart = false;
            actor.usedImages[0] = 0;
            return;
        }
        auto & primaryImage = images[actor.usedImages[0]].value();
        auto imageId = primaryImage.imageId;
        auto owner = primaryImage.owner;
        auto xc = primaryImage.xc;
        auto yc = primaryImage.yc;
        auto remapping = primaryImage.remapping;
        bool hiddenLeader = primaryImage.hiddenLeader;
        auto iScriptId = primaryImage.iScriptId;
        primaryImage = MapImage {
            .imageId = imageId,
            .owner = owner,
            .xc = xc,
            .yc = yc,
            .remapping = remapping,
            .hiddenLeader = hiddenLeader,
            .drawIfCloaked = primaryImage.drawIfCloaked
        };
        initializeImage(iScriptId, gameClock.currentTick(), actor, context.isUnit, actor.usedImages[0]);
        if ( hiddenLeader )
            primaryImage.drawFunction = MapImage::DrawFunction::None;
    }
    else
    {
        logger.error("While restarting a sprite no primary image was found!");
        actor.autoRestart = false;
        return;
    }
}

void AnimContext::initializeActor(MapActor & actor, u8 direction, u16 imageId, u8 owner, s32 xc, s32 yc, u32 iScriptId, bool isUnit, bool autoRestart, std::uint64_t drawListValue)
{
    actor.drawListIndex = drawList.size();
    drawList.push_back(drawListValue);
    actor.direction = direction;

    u16 imageIndex = createImage();
    actor.usedImages[0] = imageIndex;
    actor.autoRestart = autoRestart;
    MapImage* image = &images[imageIndex].value();
    image->imageId = imageId;
    image->owner = owner;
    image->xc = xc;
    image->yc = yc;
    initializeImage(iScriptId, gameClock.currentTick(), actor, isUnit, imageIndex);
}

void AnimContext::initSpecialCases(MapActor & actor, std::size_t type, bool isUnit, bool isSpriteUnit)
{
    if ( ((isUnit || isSpriteUnit) && type == Sc::Unit::Type::ScannerSweep) || (!isUnit && type == Sc::Sprite::Type::ScannerSweep) )
    {
        MapImage* primaryImage = actor.primaryImage(*this);
        primaryImage->hiddenLeader = true; // Mark this as a "hidden leader" image so auto-restarts can proceed smoothly
        primaryImage->drawFunction = MapImage::DrawFunction::None; // Don't draw the primary image for scanner sweep (a terran marine)
        actor.autoRestart = true; // Always auto-restart a scanner sweep
    }
    else if ( !isUnit && !isSpriteUnit && type == Sc::Sprite::Type::PsionicStorm )
    {
        MapImage* primaryImage = actor.primaryImage(*this);
        const Sc::Sprite::IScriptAnimation* specialState2 = scData.sprites.getAnimationHeader(primaryImage->iScriptId, Sc::Sprite::AnimHeader::SpecialState2);
        if ( specialState2 != nullptr )
        {
            primaryImage->animation = specialState2;
            primaryImage->waitUntil = gameClock.currentTick();
        }
    }
    else if ( ((isUnit || isSpriteUnit) && Sc::Unit::Type::ProtossNexus) || (!isUnit && type == Sc::Sprite::Type::ProtossNexus) )
    {
        MapImage* primaryImage = actor.primaryImage(*this);
        for ( auto usedImageIndex : actor.usedImages )
        {
            auto & image = images[usedImageIndex];
            if ( image && image->imageId == 181 ) // Remove the bulbs from nexus
                image->hidden = true;
        }
    }
}

void AnimContext::initializeUnitActor(MapActor & actor, bool isClipboard, std::size_t unitIndex, const Chk::Unit & unit, s32 xc, s32 yc)
{
    auto unitType = unit.type < scData.units.numUnitTypes() ? unit.type : Sc::Unit::Type::TerranMarine;
    const auto & unitDat = scData.units.getUnit(unitType);
    bool isSubUnit = unitDat.flags & Sc::Unit::Flags::Subunit;
    bool isFlyerOrLifted = unitDat.flags & Sc::Unit::Flags::Flyer ||
        unitDat.flags & Sc::Unit::Flags::FlyingBuilding && unit.isLifted();

    actor.elevation = unitDat.elevationLevel;
    u8 direction = unitDat.unitDirection == 32 ? ((std::rand() & 31) << 3) : (unitDat.unitDirection << 3);
    u32 iScriptId = iscriptIdFromUnit(scData, unitType);

    std::uint64_t drawListValue = isClipboard ?
        (std::uint64_t(unitIndex) | FlagDrawUnit | FlagUnitActor | (isSubUnit ? FlagIsTurret : 0) |
            MaskY | (std::uint64_t(actor.elevation) << ShiftElevation)) :
        (std::uint64_t(unitIndex) | FlagDrawUnit | FlagUnitActor | (isSubUnit ? FlagIsTurret : 0) |
            (std::uint64_t(yc) << ShiftY) | (std::uint64_t(actor.elevation) << ShiftElevation));

    initializeActor(actor, direction, getImageId(scData, unit), unit.owner, xc, yc, iScriptId, true, false, drawListValue);
    initSpecialCases(actor, std::size_t(unitType), true);

    // Update draw func if applicable
    if ( unit.stateFlags & Chk::Unit::State::Hallucinated )
        actor.setDrawFunction(MapImage::DrawFunction::Hallucination, *this);
    else if ( unit.stateFlags & Chk::Unit::State::Cloak )
        actor.setDrawFunction(MapImage::DrawFunction::Cloaked, *this);

    // Update anim if applicable
    if ( unitDat.flags & Sc::Unit::Flags::ResourceContainer )
        actor.setAnim(ResourceLevel::animHeader(ResourceLevel::get(unit.resourceAmount)), gameClock.currentTick(), true, *this, true);
    else if ( unitDat.flags & Sc::Unit::Flags::FlyingBuilding && unit.isLifted() )
    {
        actor.transitShadowTarget = 42;
        actor.setAnim(Sc::Sprite::AnimHeader::LiftOff, gameClock.currentTick(), true, *this);
    }
    else if ( unitDat.flags & Sc::Unit::Flags::Burrowable && unit.stateFlags & Chk::Unit::State::Burrow )
        actor.setAnim(Sc::Sprite::AnimHeader::Burrow, gameClock.currentTick(), true, *this);
    else if ( unitDat.flags & Sc::Unit::Flags::Addon && unit.relationFlags & Chk::Unit::RelationFlag::AddonLink )
        actor.setAnim(Sc::Sprite::AnimHeader::Landing, gameClock.currentTick(), true, *this);
}

void AnimContext::initializeSpriteActor(MapActor & actor, bool isClipboard, std::size_t spriteIndex, const Chk::Sprite & sprite, s32 xc, s32 yc)
{
    bool isSpriteUnit = sprite.isUnit();
    auto spriteType = isSpriteUnit ? (Sc::Unit::Type(sprite.type) < scData.units.numUnitTypes() ? sprite.type : Sc::Sprite::Type(0)) :
        (sprite.type < scData.sprites.numSprites() ? sprite.type : Sc::Sprite::Type(0));
    bool isSubUnit = isSpriteUnit && scData.units.getUnit(Sc::Unit::Type(spriteType)).flags & Sc::Unit::Flags::Subunit;
    bool isFlyer = isSpriteUnit && scData.units.getUnit(Sc::Unit::Type(spriteType)).flags & Sc::Unit::Flags::Flyer;

    actor.elevation = 4;
    if ( isSpriteUnit )
    {
        if ( sprite.flags & Chk::Sprite::SpriteFlags::SpriteUnitDiabled )
            actor.elevation = 1;
        else
            actor.elevation = scData.units.getUnit(Sc::Unit::Type(spriteType)).elevationLevel;
    }

    u32 iScriptId = isSpriteUnit ? iscriptIdFromUnit(scData, Sc::Unit::Type(spriteType)) : iscriptIdFromSprite(scData, spriteType);
    bool autoRestart = spriteType < scData.sprites.spriteAutoRestart.size() ? scData.sprites.spriteAutoRestart[spriteType] : true;
    std::uint64_t drawListValue = isClipboard ?
        (std::uint64_t(spriteIndex) | (isSubUnit ? FlagIsTurret : 0) |
            MaskY | (std::uint64_t(actor.elevation) << ShiftElevation)) :
        (std::uint64_t(spriteIndex) | (isSpriteUnit ? FlagDrawUnit : 0) | (isSubUnit ? FlagIsTurret : 0) |
            (std::uint64_t(yc) << ShiftY) | (std::uint64_t(actor.elevation) << ShiftElevation));

    initializeActor(actor, 0, getImageId(scData, sprite), sprite.owner, xc, yc, iScriptId, false, autoRestart, drawListValue);
    initSpecialCases(actor, std::size_t(spriteType), false, isSpriteUnit);

    // Update anim if applicable
    if ( isSpriteUnit )
    {
        if ( sprite.flags & Chk::Sprite::SpriteFlags::SpriteUnitDiabled )
            actor.setAnim(Sc::Sprite::AnimHeader::Disable, gameClock.currentTick(), false, *this, true);
        else
            actor.setAnim(Sc::Sprite::AnimHeader::Enable, gameClock.currentTick(), false, *this, true);
    }
}

MapAnimations::MapAnimations(const Sc::Data & scData, const GameClock & gameClock, const Scenario & scenario)
    : AnimContext(scData, gameClock), scenario(scenario)
{

}

void MapAnimations::addUnit(std::size_t unitIndex, MapActor & actor)
{
    drawListDirty = true;
    const auto & unitActors = scenario.view.units.readAttachedData();
    for ( std::size_t i=unitIndex; i<unitActors.size(); ++i )
        ++drawList[unitActors[i].drawListIndex]; // Increment affected unit indexes in draw list

    const auto & unit = scenario.getUnit(unitIndex);
    initializeUnitActor(actor, false, unitIndex, unit, unit.xc, unit.yc);
}

void MapAnimations::addSprite(std::size_t spriteIndex, MapActor & actor)
{
    drawListDirty = true;
    const auto & spriteActors = scenario.view.sprites.readAttachedData();
    for ( std::size_t i=spriteIndex; i<spriteActors.size(); ++i )
        ++drawList[spriteActors[i].drawListIndex]; // Increment affected sprite indexes in draw list

    const auto & sprite = scenario.getSprite(spriteIndex);
    initializeSpriteActor(actor, false, spriteIndex, sprite, sprite.xc, sprite.yc);
}

void MapAnimations::removeUnit(std::size_t unitIndex, MapActor & actor)
{
    drawListDirty = true;
    const auto & unitActors = scenario.view.units.readAttachedData();
    for ( std::size_t i=unitIndex+1; i<unitActors.size(); ++i )
    {
        if ( drawList[unitActors[i].drawListIndex] != UnusedDrawEntry )
            --drawList[unitActors[i].drawListIndex]; // Decrement affected unit indexes in draw list
    }

    drawList[actor.drawListIndex] = UnusedDrawEntry;
    actor.drawListIndex = 0;
    for ( auto usedImage : actor.usedImages )
        removeImage(usedImage);
}

void MapAnimations::removeSprite(std::size_t spriteIndex, MapActor & actor)
{
    drawListDirty = true;
    const auto & spriteActors = scenario.view.sprites.readAttachedData();
    for ( std::size_t i=spriteIndex+1; i<spriteActors.size(); ++i )
    {
        if ( drawList[spriteActors[i].drawListIndex] != UnusedDrawEntry )
            --drawList[spriteActors[i].drawListIndex]; // Decrement affected sprite indexes in draw list
    }
    
    drawList[actor.drawListIndex] = UnusedDrawEntry;
    actor.drawListIndex = 0;
    for ( auto usedImage : actor.usedImages )
        removeImage(usedImage);
}

void MapAnimations::updateUnitType(std::size_t unitIndex, Sc::Unit::Type newUnitType)
{
    drawListDirty = true;
    const auto & unit = scenario.read.units[unitIndex];
    auto & unitActor = scenario.view.units.attachedData(unitIndex);
    clearActor(unitActor);
    initializeUnitActor(unitActor, false, unitIndex, unit, unit.xc, unit.yc);
}

void MapAnimations::updateSpriteType(std::size_t spriteIndex, Sc::Sprite::Type newSpriteType)
{
    drawListDirty = true;
    const auto & sprite = scenario.read.sprites[spriteIndex];
    auto & spriteActor = scenario.view.sprites.attachedData(spriteIndex);
    clearActor(spriteActor);
    initializeSpriteActor(spriteActor, false, spriteIndex, sprite, sprite.xc, sprite.yc);
}

void MapAnimations::updateUnitOwner(std::size_t unitIndex, u8 newUnitOwner)
{
    auto & unitActor = scenario.view.units.attachedData(unitIndex);
    for ( auto usedImageIndex : unitActor.usedImages )
    {
        if ( usedImageIndex != 0 )
            images[usedImageIndex]->owner = newUnitOwner;
    }
}

void MapAnimations::updateSpriteOwner(std::size_t spriteIndex, u8 newSpriteOwner)
{
    auto & spriteActor = scenario.view.sprites.attachedData(spriteIndex);
    for ( auto usedImageIndex : spriteActor.usedImages )
    {
        if ( usedImageIndex != 0 )
            images[usedImageIndex]->owner = newSpriteOwner;
    }
}

void MapAnimations::updateUnitIndex(std::size_t unitIndexFrom, std::size_t unitIndexTo)
{
    auto & unitActor = scenario.view.units.attachedData(unitIndexTo);
    std::uint64_t & drawListEntry = drawList[unitActor.drawListIndex];
    drawListEntry = unitIndexTo | (drawListEntry & MaskNonIndexDrawList);
}

void MapAnimations::updateSpriteIndex(std::size_t spriteIndexFrom, std::size_t spriteIndexTo)
{
    auto & spriteActor = scenario.view.sprites.attachedData(spriteIndexTo);
    std::uint64_t & drawListEntry = drawList[spriteActor.drawListIndex];
    drawListEntry = spriteIndexTo | (drawListEntry & MaskNonIndexDrawList);
}

void MapAnimations::updateUnitXc(std::size_t unitIndex, u16 oldXc, u16 newXc)
{
    auto & actor = scenario.view.units.attachedData(unitIndex);
    s32 difference = s32(newXc) - s32(oldXc);
    for ( auto usedImageIndex : actor.usedImages )
    {
        if ( usedImageIndex != 0 )
            images[usedImageIndex]->xc += difference;
    }
}

void MapAnimations::updateUnitYc(std::size_t unitIndex, u16 oldYc, u16 newYc)
{
    auto & actor = scenario.view.units.attachedData(unitIndex);
    s32 difference = s32(newYc) - s32(oldYc);
    for ( auto usedImageIndex : actor.usedImages )
    {
        if ( usedImageIndex != 0 )
            images[usedImageIndex]->yc += difference;
    }
}

void MapAnimations::updateSpriteXc(std::size_t spriteIndex, u16 oldXc, u16 newXc)
{
    auto & actor = scenario.view.sprites.attachedData(spriteIndex);
    s32 difference = s32(newXc) - s32(oldXc);
    for ( auto usedImageIndex : actor.usedImages )
    {
        if ( usedImageIndex != 0 )
            images[usedImageIndex]->xc += difference;
    }
}

void MapAnimations::updateSpriteYc(std::size_t spriteIndex, u16 oldYc, u16 newYc)
{
    auto & actor = scenario.view.sprites.attachedData(spriteIndex);
    s32 difference = s32(newYc) - s32(oldYc);
    for ( auto usedImageIndex : actor.usedImages )
    {
        if ( usedImageIndex != 0 )
            images[usedImageIndex]->yc += difference;
    }
}

void MapAnimations::updateUnitResourceAmount(std::size_t unitIndex, u32 oldResourceAmount, u32 newResourceAmount)
{
    const auto & unit = scenario.read.units[unitIndex];
    const auto & unitDat = scData.units.getUnit(unit.type);
    auto & actor = scenario.view.units.attachedData(unitIndex);
    
    if ( unitDat.flags & Sc::Unit::Flags::ResourceContainer )
    {
        auto newResourceLevel = ResourceLevel::get(newResourceAmount);
        if ( ResourceLevel::get(oldResourceAmount) != newResourceLevel )
            actor.setAnim(ResourceLevel::animHeader(newResourceLevel), gameClock.currentTick(), true, *this, true);
    }
}

void MapAnimations::updateUnitStateFlags(std::size_t unitIndex, u16 oldStateFlags, u16 newStateFlags)
{
    const auto & unit = scenario.read.units[unitIndex];
    const auto & unitDat = scData.units.getUnit(unit.type);
    auto & actor = scenario.view.units.attachedData(unitIndex);

    bool wasLifted = oldStateFlags & Chk::Unit::State::InTransit;
    bool isLifted = newStateFlags & Chk::Unit::State::InTransit;
    if ( wasLifted != isLifted && unitDat.flags & Sc::Unit::Flags::FlyingBuilding )
    {
        if ( isLifted )
        {
            actor.transitShadowTarget = 42;
            actor.setAnim(Sc::Sprite::AnimHeader::LiftOff, gameClock.currentTick(), true, *this);
        }
        else
        {
            actor.transitShadowTarget = 0;
            actor.setAnim(Sc::Sprite::AnimHeader::Landing, gameClock.currentTick(), true, *this);
        }
    }

    bool wasBurrowed = oldStateFlags & Chk::Unit::State::Burrow;
    bool isBurrowed = newStateFlags & Chk::Unit::State::Burrow;
    if ( wasBurrowed != isBurrowed && unitDat.flags & Sc::Unit::Flags::Burrowable )
    {
        if ( isBurrowed )
            actor.setAnim(Sc::Sprite::AnimHeader::Burrow, gameClock.currentTick(), true, *this);
        else
            actor.setAnim(Sc::Sprite::AnimHeader::Unburrow, gameClock.currentTick(), true, *this);
    }

    bool wasHallucinated = oldStateFlags & Chk::Unit::State::Hallucinated;
    bool isHallucinated = newStateFlags & Chk::Unit::State::Hallucinated;
    bool wasCloaked = oldStateFlags & Chk::Unit::State::Cloak;
    bool isCloaked = newStateFlags & Chk::Unit::State::Cloak;
    if ( wasHallucinated != isHallucinated )
    {
        if ( isHallucinated )
            actor.setDrawFunction(MapImage::DrawFunction::Hallucination, *this);
        else
            actor.setDrawFunction(isCloaked ? MapImage::DrawFunction::Cloaked : MapImage::DrawFunction::Normal, *this);
    }
    else if ( !isHallucinated && wasCloaked != isCloaked )
        actor.setDrawFunction(isCloaked ? MapImage::DrawFunction::Cloaked : MapImage::DrawFunction::Normal, *this);
}

void MapAnimations::updateUnitRelationFlags(std::size_t unitIndex, u16 oldRelationFlags, u16 newRelationFlags)
{
    const auto & unit = scenario.read.units[unitIndex];
    const auto & unitDat = scData.units.getUnit(unit.type);
    auto & actor = scenario.view.units.attachedData(unitIndex);

    bool wasAttached = oldRelationFlags & Chk::Unit::RelationFlag::AddonLink;
    bool isAttached = newRelationFlags & Chk::Unit::RelationFlag::AddonLink;
    if ( wasAttached != isAttached && (unitDat.flags & Sc::Unit::Flags::Addon) )
    {
        if ( isAttached )
            actor.setAnim(Sc::Sprite::AnimHeader::Landing, gameClock.currentTick(), true, *this);
        else
            actor.setAnim(Sc::Sprite::AnimHeader::LiftOff, gameClock.currentTick(), true, *this);
    }
}

void MapAnimations::updateSpriteFlags(std::size_t spriteIndex, u16 oldSpriteFlags, u16 newSpriteFlags)
{
    const auto & sprite = scenario.read.sprites[spriteIndex];
    auto & actor = scenario.view.sprites.attachedData(spriteIndex);
    if ( (oldSpriteFlags & Chk::Sprite::SpriteFlags::DrawAsSprite) != (newSpriteFlags & Chk::Sprite::SpriteFlags::DrawAsSprite) )
    {
        drawListDirty = true;
        clearActor(actor);
        initializeSpriteActor(actor, false, spriteIndex, sprite, sprite.xc, sprite.yc);
    }
    if ( sprite.isUnit() && (oldSpriteFlags & Chk::Sprite::SpriteFlags::SpriteUnitDiabled) != (newSpriteFlags & Chk::Sprite::SpriteFlags::SpriteUnitDiabled) )
    {
        auto oldElevation = actor.elevation;
        if ( newSpriteFlags & Chk::Sprite::SpriteFlags::SpriteUnitDiabled )
        {
            actor.elevation = 1;
            actor.setAnim(Sc::Sprite::AnimHeader::Disable, gameClock.currentTick(), false, *this, true);
        }
        else
        {
            actor.elevation = scData.units.getUnit(Sc::Unit::Type(sprite.type)).elevationLevel;
            actor.setAnim(Sc::Sprite::AnimHeader::Enable, gameClock.currentTick(), false, *this, true);
        }

        if ( actor.elevation != oldElevation )
        {
            std::uint64_t & drawListValue = drawList[actor.drawListIndex];
            drawListValue = (drawListValue & (~MaskElevation)) | (std::uint64_t(actor.elevation) << ShiftElevation);
            drawListDirty = true;
        }
    }
}

void MapAnimations::cleanDrawList()
{
    if ( drawListDirty )
    {
        std::sort(drawList.begin(), drawList.end());
        for ( std::size_t i=1; i<drawList.size(); ++i )
        {
            if ( drawList[i] == UnusedDrawEntry )
                break;
            else
            {
                std::uint64_t drawItem = drawList[i];
                std::uint32_t index = std::uint32_t(drawItem & MaskIndex);
                if ( drawItem & FlagUnitActor )
                    scenario.view.units.attachedData(index).drawListIndex = i;
                else
                    scenario.view.sprites.attachedData(index).drawListIndex = i;
            }
        }
        drawListDirty = false;
    }
}

void MapAnimations::animate(uint64_t currentTick)
{
    std::size_t unitSize = scenario.view.units.readAttachedData().size();
    std::size_t spriteSize = scenario.view.sprites.readAttachedData().size();
    for ( std::size_t i=0; i<unitSize; ++i )
        scenario.view.units.attachedData(i).animate(currentTick, true, *this);
    
    for ( std::size_t i=0; i<spriteSize; ++i )
        scenario.view.sprites.attachedData(i).animate(currentTick, false, *this);

    cleanDrawList();
}
