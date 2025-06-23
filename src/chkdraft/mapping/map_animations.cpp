#include "map_animations.h"
#include "chkdraft.h"

u32 iscriptIdFromUnit(Sc::Unit::Type unitType)
{
    return chkd.scData.sprites.getImage(
        chkd.scData.sprites.getSprite(
            chkd.scData.units.getFlingy(
                chkd.scData.units.getUnit(
                    size_t(unitType) < Sc::Unit::TotalTypes ? unitType : Sc::Unit::Type(0)
                ).graphics
            ).sprite
        ).imageFile
    ).iScriptId;
}

u32 iscriptIdFromSprite(Sc::Sprite::Type spriteType)
{
    return chkd.scData.sprites.getImage(
        chkd.scData.sprites.getSprite(
            size_t(spriteType) < Sc::Sprite::TotalSprites ? spriteType : Sc::Sprite::Type(0)
        ).imageFile
    ).iScriptId;
}

size_t getImageId(Sc::Unit::Type unitType)
{
    u32 flingyId = u32(chkd.scData.units.getUnit(Sc::Unit::Type(unitType >= 228 ? 0 : unitType)).graphics);
    u32 spriteId = u32(chkd.scData.units.getFlingy(flingyId >= 209 ? 0 : flingyId).sprite);
    return chkd.scData.sprites.getSprite(spriteId >= 517 ? 0 : spriteId).imageFile;
}

size_t getImageId(Sc::Sprite::Type spriteType)
{
    return chkd.scData.sprites.getSprite(spriteType >= 517 ? 0 : spriteType).imageFile;
}

size_t getImageId(const Chk::Unit & unit)
{
    return getImageId(unit.type);
}

size_t getImageId(Sc::Sprite::Type spriteType, bool isDrawnAsSprite)
{
    return isDrawnAsSprite ?
        getImageId(spriteType) :
        getImageId(Sc::Unit::Type(spriteType));
}

size_t getImageId(const Chk::Sprite & sprite)
{
    return sprite.isDrawnAsSprite() ?
        getImageId(sprite.type) :
        getImageId(Sc::Unit::Type(sprite.type));
}

u16 MapAnimations::createImage()
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

void MapAnimations::removeImage(u16 imageIndex)
{
    if ( imageIndex == 0 )
        return;

    images[imageIndex] = std::nullopt;
    availableImages.push_back(imageIndex);
}

void MapAnimations::initializeImage(u32 iScriptId, std::uint64_t currentTick, MapActor & actor, bool isUnit, std::size_t imageIndex)
{
    AnimationContext context {
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

MapAnimations::MapAnimations(const Scenario & scenario, Clipboard & clipboard)
    : scenario(scenario), clipboard(clipboard), images{MapImage{}} // Image 0 is unused
{

}

void MapAnimations::initClipboardUnits()
{
    clearClipboardUnits();
    const auto & pasteUnits = clipboard.getUnits();
    if ( !pasteUnits.empty() )
        drawListDirty = true;

    for ( std::size_t i=0; i<pasteUnits.size(); ++i ) // TODO: should sort pasteUnits by draw order first
    {
        const auto & pasteUnit = pasteUnits[i];
        initializeUnitActor(clipboard.unitActors.emplace_back(), true, i, pasteUnit.unit, pasteUnit.xc, pasteUnit.yc);
    }
}

void MapAnimations::initClipboardSprites()
{
    clearClipboardSprites();
    const auto & pasteSprites = clipboard.getSprites();
    if ( !pasteSprites.empty() )
        drawListDirty = true;

    for ( std::size_t i=0; i<pasteSprites.size(); ++i ) // TODO: should sort pasteSprites by draw order first
    {
        const auto & pasteSprite = pasteSprites[i];
        initializeSpriteActor(clipboard.spriteActors.emplace_back(), true, i, pasteSprite.sprite, pasteSprite.xc, pasteSprite.yc);
    }
}

void MapAnimations::clearClipboardUnits()
{
    drawListDirty = true;
    for ( auto & unitActor : clipboard.unitActors )
    {
        drawList[unitActor.drawListIndex] = UnusedDrawEntry;
        for ( auto usedImage : unitActor.usedImages )
            removeImage(usedImage);
    }
    clipboard.unitActors.clear();
}

void MapAnimations::clearClipboardSprites()
{
    drawListDirty = true;
    for ( auto & spriteActor : clipboard.spriteActors )
    {
        drawList[spriteActor.drawListIndex] = UnusedDrawEntry;
        for ( auto usedImage : spriteActor.usedImages )
            removeImage(usedImage);
    }
    clipboard.spriteActors.clear();
}

void MapAnimations::clearClipboardActors()
{
    clearClipboardUnits();
    clearClipboardSprites();
}

void MapAnimations::restartActor(AnimationContext & context)
{
    auto & actor = context.actor;
    for ( std::size_t i=0; i<std::size(actor.usedImages); ++i ) // Remove all non-primary actors
    {
        if ( i != actor.primaryImageIndex )
        {
            removeImage(actor.usedImages[i]);
            actor.usedImages[i] = 0;
        }
    }
    if ( actor.primaryImageIndex < std::size(actor.usedImages) )
    {
        actor.usedImages[0] = actor.usedImages[actor.primaryImageIndex];
        actor.primaryImageIndex = 0;
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
        bool hiddenLeader = primaryImage.hiddenLeader;
        auto iScriptId = primaryImage.iScriptId;
        primaryImage = MapImage {
            .imageId = imageId,
            .owner = owner,
            .xc = xc,
            .yc = yc,
            .hiddenLeader = hiddenLeader
        };
        initializeImage(iScriptId, chkd.gameClock.currentTick(), actor, context.isUnit, actor.usedImages[0]);
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

void MapAnimations::initializeActor(MapActor & actor, u8 direction, u16 imageId, u8 owner, s32 xc, s32 yc, u32 iScriptId, bool isUnit, bool autoRestart, std::uint64_t drawListValue)
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
    initializeImage(iScriptId, chkd.gameClock.currentTick(), actor, isUnit, imageIndex);
}

void MapAnimations::initSpecialCases(MapActor & actor, std::size_t type, bool isUnit, bool isSpriteUnit)
{
    if ( ((isUnit || isSpriteUnit) && type == Sc::Unit::Type::ScannerSweep) || (!isUnit && type == 237 /*TODO: enum sprites, this is scanner sweep sprite*/) )
    {
        MapImage* primaryImage = actor.primaryImage(*this);
        primaryImage->hiddenLeader = true; // Mark this as a "hidden leader" image so auto-restarts can proceed smoothly
        primaryImage->drawFunction = MapImage::DrawFunction::None; // Don't draw the primary image for scanner sweep (a terran marine)
        actor.autoRestart = true; // Always auto-restart a scanner sweep
    }
    else if ( !isUnit && !isSpriteUnit && type == 339 /*TODO: enum sprites, this is psionic storm sprite*/ )
    {
        MapImage* primaryImage = actor.primaryImage(*this);
        const Sc::Sprite::IScriptAnimation* specialState2 = chkd.scData.sprites.getAnimationHeader(primaryImage->iScriptId, Sc::Sprite::AnimHeader::SpecialState2);
        if ( specialState2 != nullptr )
        {
            primaryImage->animation = specialState2;
            primaryImage->waitUntil = chkd.gameClock.currentTick();
        }
    }
}

void MapAnimations::initializeUnitActor(MapActor & actor, bool isClipboard, std::size_t unitIndex, const Chk::Unit & unit, s32 xc, s32 yc)
{
    const auto & unitDat = chkd.scData.units.getUnit(unit.type);
    bool isSubUnit = unitDat.flags & Sc::Unit::Flags::Subunit;
    bool isFlyerOrLifted = unitDat.flags & Sc::Unit::Flags::Flyer ||
        unitDat.flags & Sc::Unit::Flags::FlyingBuilding && unit.isLifted();

    std::uint64_t elevation = isFlyerOrLifted ? 1 : 0; // TODO: tile elevations
    u8 direction = unitDat.unitDirection == 32 ? ((std::rand() & 31) << 3) : (unitDat.unitDirection << 3);
    u32 iScriptId = iscriptIdFromUnit(unit.type);

    std::size_t drawListValue = isClipboard ?
        (std::uint64_t(unitIndex) | FlagDrawUnit | FlagIsClipboard | FlagUnitActor | (isSubUnit ? FlagIsTurret : 0) |
            MaskY | (elevation << ShiftElevation)) :
        (std::uint64_t(unitIndex) | FlagDrawUnit | FlagUnitActor | (isSubUnit ? FlagIsTurret : 0) |
            (std::uint64_t(yc) << ShiftY) | (elevation << ShiftElevation));

    initializeActor(actor, direction, getImageId(unit), unit.owner, xc, yc, iScriptId, true, false, drawListValue);
    initSpecialCases(actor, std::size_t(unit.type), true);
}

void MapAnimations::initializeSpriteActor(MapActor & actor, bool isClipboard, std::size_t spriteIndex, const Chk::Sprite & sprite, s32 xc, s32 yc)
{
    bool isSpriteUnit = sprite.isUnit();
    bool isSubUnit = isSpriteUnit && chkd.scData.units.getUnit(Sc::Unit::Type(sprite.type)).flags & Sc::Unit::Flags::Subunit;
    bool isFlyer = isSpriteUnit && chkd.scData.units.getUnit(Sc::Unit::Type(sprite.type)).flags & Sc::Unit::Flags::Flyer;

    std::uint64_t elevation = isFlyer ? 1 : 0; // TODO: tile elevations
    u32 iScriptId = iscriptIdFromSprite(sprite.type);
    bool autoRestart = sprite.type < chkd.scData.sprites.spriteAutoRestart.size() ? chkd.scData.sprites.spriteAutoRestart[sprite.type] : true;
    std::size_t drawListValue = isClipboard ?
        (std::uint64_t(spriteIndex) | FlagIsClipboard | (isSubUnit ? FlagIsTurret : 0) |
            MaskY | (elevation << ShiftElevation)) :
        (std::uint64_t(spriteIndex) | (isSpriteUnit ? FlagDrawUnit : 0) | (isSubUnit ? FlagIsTurret : 0) |
            (std::uint64_t(yc) << ShiftY) | (elevation << ShiftElevation));

    initializeActor(actor, 0, getImageId(sprite), sprite.owner, xc, yc, iScriptId, false, autoRestart, drawListValue);
    initSpecialCases(actor, std::size_t(sprite.type), false, isSpriteUnit);
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
        --drawList[unitActors[i].drawListIndex]; // Decrement affected unit indexes in draw list

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
        --drawList[spriteActors[i].drawListIndex]; // Decrement affected sprite indexes in draw list
    
    drawList[actor.drawListIndex] = UnusedDrawEntry;
    actor.drawListIndex = 0;
    for ( auto usedImage : actor.usedImages )
        removeImage(usedImage);
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
                if ( drawItem & FlagIsClipboard )
                {
                    if ( drawItem & FlagUnitActor )
                        clipboard.unitActors[index].drawListIndex = i;
                    else
                        clipboard.spriteActors[index].drawListIndex = i;
                }
                else if ( drawItem & FlagUnitActor )
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
    
    for ( auto & unitActor : clipboard.unitActors )
        unitActor.animate(currentTick, true, *this);

    for ( auto & spriteActor : clipboard.spriteActors )
        spriteActor.animate(currentTick, false, *this);

    cleanDrawList();
}
