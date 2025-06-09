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

MapAnimations::MapAnimations(const Scenario & scenario)
    : scenario(scenario), images{MapImage{}} // Image 0 is unused
{

}

void MapAnimations::initialize()
{
    images = {MapImage{}};
    spriteActors.clear();
    unitActors.clear();
    availableImages.clear();

    auto currentTick = chkd.gameClock.currentTick();
    for ( const auto & unit : scenario->units )
    {
        auto & actor = unitActors.emplace_back(images);
        actor.initialize(currentTick, iscriptIdFromUnit(unit.type), true);
        actor.usedImages[0] = images.size();
        auto & image = *images.emplace_back(MapImage{});
        image.imageId = getImageId(unit);
        image.owner = unit.owner;
        image.xc = unit.xc;
        image.yc = unit.yc;
    }

    for ( const auto & sprite : scenario->sprites )
    {
        auto & actor = spriteActors.emplace_back(images);
        actor.initialize(currentTick, iscriptIdFromSprite(sprite.type), false);
        actor.usedImages[0] = images.size();
        auto & image = *images.emplace_back(MapImage{});
        image.imageId = getImageId(sprite);
        image.owner = sprite.owner;
        image.xc = sprite.xc;
        image.yc = sprite.yc;
    }
}

void MapAnimations::addUnit(std::size_t unitIndex)
{
    const auto & unit = scenario.getUnit(unitIndex);
    auto & actor = unitActors.emplace_back(images);
    actor.initialize(chkd.gameClock.currentTick(), iscriptIdFromUnit(unit.type), true);
    u16 imageIndex = createImage();
    actor.usedImages[0] = imageIndex;
    MapImage & image = images[imageIndex].value();
    image.imageId = getImageId(unit);
    image.owner = unit.owner;
    image.xc = unit.xc;
    image.yc = unit.yc;
}

void MapAnimations::addSprite(std::size_t spriteIndex)
{
    const auto & sprite = scenario.getSprite(spriteIndex);
    auto & actor = spriteActors.emplace_back(images);
    actor.initialize(chkd.gameClock.currentTick(), iscriptIdFromSprite(sprite.type), false);
    u16 imageIndex = createImage();
    actor.usedImages[0] = imageIndex;
    MapImage & image = images[imageIndex].value();
    image.imageId = getImageId(sprite);
    image.owner = sprite.owner;
    image.xc = sprite.xc;
    image.yc = sprite.yc;
}

void MapAnimations::removeUnit(std::size_t unitIndex)
{
    auto removedActor = std::next(unitActors.begin(), unitIndex);
    for ( auto usedImage : removedActor->usedImages )
        removeImage(usedImage);

    unitActors.erase(removedActor);
}

void MapAnimations::removeSprite(std::size_t spriteIndex)
{
    auto removedActor = std::next(spriteActors.begin(), spriteIndex);
    for ( auto usedImage : removedActor->usedImages )
        removeImage(usedImage);

    spriteActors.erase(removedActor);
}

void MapAnimations::animate(uint64_t currentTick)
{
    for ( auto & clipboardSprite : CM->clipboard.getSprites() )
        clipboardSprite.testAnim.animate(currentTick, false);

    for ( auto & actor : unitActors )
        actor.animate(currentTick, true);

    for ( auto & actor : spriteActors )
        actor.animate(currentTick, false);
}
