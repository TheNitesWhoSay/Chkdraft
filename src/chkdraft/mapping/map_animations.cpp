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

MapAnimations::MapAnimations()
    : images {MapImage{}} // Image 0 is unused
{

}

void MapAnimations::initialize(const Scenario & scenario)
{
    images = {MapImage{}};
    spriteActors.clear();
    unitActors.clear();

    auto currentTick = chkd.gameClock.currentTick();
    for ( const auto & unit : scenario.units )
    {
        auto & actor = unitActors.emplace_back(images);
        actor.initialize(currentTick, iscriptIdFromUnit(unit.type));
        actor.usedImages[0] = images.size();
        auto & image = images.emplace_back();
        image.imageId = getImageId(unit);
        image.owner = unit.owner;
        image.xc = unit.xc;
        image.yc = unit.yc;
    }

    for ( const auto & sprite : scenario.sprites )
    {
        auto & actor = spriteActors.emplace_back(images);
        actor.initialize(currentTick, iscriptIdFromSprite(sprite.type));
        actor.usedImages[0] = images.size();
        auto & image = images.emplace_back();
        image.imageId = getImageId(sprite);
        image.owner = sprite.owner;
        image.xc = sprite.xc;
        image.yc = sprite.yc;
    }
}

void MapAnimations::animate(uint64_t currentTick)
{
    for ( auto & actor : unitActors )
        actor.animate(currentTick);

    for ( auto & actor : spriteActors )
        actor.animate(currentTick);
}
