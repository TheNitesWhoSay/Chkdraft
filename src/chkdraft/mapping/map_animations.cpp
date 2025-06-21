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
        auto & actor = clipboard.unitActors.emplace_back();
        bool isSubUnit = chkd.scData.units.getUnit(pasteUnit.unit.type).flags & Sc::Unit::Flags::Subunit;
        bool isFlyerOrLifted = chkd.scData.units.getUnit(pasteUnit.unit.type).flags & Sc::Unit::Flags::Flyer ||
            (chkd.scData.units.getUnit(pasteUnit.unit.type).flags & Sc::Unit::Flags::FlyingBuilding && pasteUnit.unit.isLifted());
        std::uint64_t elevation = isFlyerOrLifted ? 1 : 0; // TODO: tile elevations
        actor.drawListIndex = drawList.size();
        drawList.push_back(
            std::uint64_t(i) | FlagDrawUnit | FlagIsClipboard | FlagUnitActor | (isSubUnit ? FlagIsTurret : 0) |
            MaskY | (elevation << ShiftElevation)
        );

        u16 imageIndex = createImage();
        actor.usedImages[0] = imageIndex;
        MapImage* image = &images[imageIndex].value();
        image->imageId = getImageId(pasteUnit.unit);
        image->owner = pasteUnit.unit.owner;
        image->xc = pasteUnit.xc;
        image->yc = pasteUnit.yc;
        initializeImage(iscriptIdFromUnit(pasteUnit.unit.type), chkd.gameClock.currentTick(), actor, true, imageIndex);
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
        auto & actor = clipboard.spriteActors.emplace_back();
        bool isSpriteUnit = pasteSprite.sprite.isUnit();
        bool isSubUnit = isSpriteUnit && chkd.scData.units.getUnit(Sc::Unit::Type(pasteSprite.sprite.type)).flags & Sc::Unit::Flags::Subunit;
        bool isFlyer = isSpriteUnit && chkd.scData.units.getUnit(Sc::Unit::Type(pasteSprite.sprite.type)).flags & Sc::Unit::Flags::Flyer;
        std::uint64_t elevation = isFlyer ? 1 : 0; // TODO: tile elevations
        actor.drawListIndex = drawList.size();
        drawList.push_back(
            std::uint64_t(i) | FlagIsClipboard | (isSubUnit ? FlagIsTurret : 0) |
            MaskY | (elevation << ShiftElevation)
        );

        u16 imageIndex = createImage();
        actor.usedImages[0] = imageIndex;
        MapImage* image = &images[imageIndex].value();
        image->imageId = getImageId(pasteSprite.sprite);
        image->owner = pasteSprite.sprite.owner;
        image->xc = pasteSprite.xc;
        image->yc = pasteSprite.yc;
        initializeImage(iscriptIdFromSprite(pasteSprite.sprite.type), chkd.gameClock.currentTick(), actor, false, imageIndex);
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

void MapAnimations::addUnit(std::size_t unitIndex, MapActor & actor)
{
    drawListDirty = true;
    const auto & unitActors = scenario.view.units.readAttachedData();
    for ( std::size_t i=unitIndex; i<unitActors.size(); ++i )
        ++drawList[unitActors[i].drawListIndex]; // Increment affected unit indexes in draw list

    const auto & unit = scenario.getUnit(unitIndex);
    const auto & unitDat = chkd.scData.units.getUnit(unit.type);
    bool isSubUnit = unitDat.flags & Sc::Unit::Flags::Subunit;
    bool isFlyerOrLifted = unitDat.flags & Sc::Unit::Flags::Flyer ||
        unitDat.flags & Sc::Unit::Flags::FlyingBuilding && unit.isLifted();
    std::uint64_t elevation = isFlyerOrLifted ? 1 : 0; // TODO: tile elevations
    actor.drawListIndex = drawList.size();
    drawList.push_back(
        std::uint64_t(unitIndex) | FlagDrawUnit | FlagUnitActor | (isSubUnit ? FlagIsTurret : 0) |
        (std::uint64_t(unit.yc) << ShiftY) | (elevation << ShiftElevation)
    );
    if ( unitDat.unitDirection == 32 )
        actor.direction = (std::rand() & 31) << 3;
    else
        actor.direction = unitDat.unitDirection << 3;

    u16 imageIndex = createImage();
    actor.usedImages[0] = imageIndex;
    MapImage* image = &images[imageIndex].value();
    image->imageId = getImageId(unit);
    image->owner = unit.owner;
    image->xc = unit.xc;
    image->yc = unit.yc;
    initializeImage(iscriptIdFromUnit(unit.type), chkd.gameClock.currentTick(), actor, true, imageIndex);
}

void MapAnimations::addSprite(std::size_t spriteIndex, MapActor & actor)
{
    drawListDirty = true;
    const auto & spriteActors = scenario.view.sprites.readAttachedData();
    for ( std::size_t i=spriteIndex; i<spriteActors.size(); ++i )
        ++drawList[spriteActors[i].drawListIndex]; // Increment affected sprite indexes in draw list

    const auto & sprite = scenario.getSprite(spriteIndex);
    bool isSpriteUnit = sprite.isUnit();
    bool isSubUnit = isSpriteUnit && chkd.scData.units.getUnit(Sc::Unit::Type(sprite.type)).flags & Sc::Unit::Flags::Subunit;
    bool isFlyer = isSpriteUnit && chkd.scData.units.getUnit(Sc::Unit::Type(sprite.type)).flags & Sc::Unit::Flags::Flyer;
    actor.drawListIndex = drawList.size();
    std::uint64_t elevation = isFlyer ? 1 : 0; // TODO: tile elevations
    drawList.push_back(
        std::uint64_t(spriteIndex) | (isSpriteUnit ? FlagDrawUnit : 0) | (isSubUnit ? FlagIsTurret : 0) |
        (std::uint64_t(sprite.yc) << ShiftY) | (elevation << ShiftElevation)
    );
    u16 imageIndex = createImage();
    actor.usedImages[0] = imageIndex;
    MapImage* image = &images[imageIndex].value();
    image->imageId = getImageId(sprite);
    image->owner = sprite.owner;
    image->xc = sprite.xc;
    image->yc = sprite.yc;
    initializeImage(iscriptIdFromSprite(sprite.type), chkd.gameClock.currentTick(), actor, false, imageIndex);
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
        spriteActor.animate(currentTick, true, *this);

    cleanDrawList();
}
