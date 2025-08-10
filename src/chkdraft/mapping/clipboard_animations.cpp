#include "clipboard_animations.h"
#include "clipboard.h"

ClipboardAnimations::ClipboardAnimations(const Sc::Data & scData, const GameClock & gameClock, Clipboard & clipboard)
    : AnimContext(scData, gameClock), clipboard(clipboard)
{

}

const std::vector<MapActor> & ClipboardAnimations::getUnitActors()
{
    return unitActors;
}

const std::vector<MapActor> & ClipboardAnimations::getSpriteActors()
{
    return spriteActors;
}

void ClipboardAnimations::initClipboardUnits()
{
    clearClipboardUnits();
    const auto & pasteUnits = clipboard.getUnits();
    if ( !pasteUnits.empty() )
        drawListDirty = true;

    for ( std::size_t i=0; i<pasteUnits.size(); ++i ) // TODO: should sort pasteUnits by draw order first
    {
        const auto & pasteUnit = pasteUnits[i];
        initializeUnitActor(unitActors.emplace_back(), true, i, pasteUnit.unit, pasteUnit.xc, pasteUnit.yc);
    }
}

void ClipboardAnimations::initClipboardSprites()
{
    clearClipboardSprites();
    const auto & pasteSprites = clipboard.getSprites();
    if ( !pasteSprites.empty() )
        drawListDirty = true;

    for ( std::size_t i=0; i<pasteSprites.size(); ++i ) // TODO: should sort pasteSprites by draw order first
    {
        const auto & pasteSprite = pasteSprites[i];
        initializeSpriteActor(spriteActors.emplace_back(), true, i, pasteSprite.sprite, pasteSprite.xc, pasteSprite.yc);
    }
}

void ClipboardAnimations::clearClipboardUnits()
{
    drawListDirty = true;
    for ( auto & unitActor : unitActors )
    {
        drawList[unitActor.drawListIndex] = UnusedDrawEntry;
        for ( auto usedImage : unitActor.usedImages )
            removeImage(usedImage);
    }
    unitActors.clear();
}

void ClipboardAnimations::clearClipboardSprites()
{
    drawListDirty = true;
    for ( auto & spriteActor : spriteActors )
    {
        drawList[spriteActor.drawListIndex] = UnusedDrawEntry;
        for ( auto usedImage : spriteActor.usedImages )
            removeImage(usedImage);
    }
    spriteActors.clear();
}

void ClipboardAnimations::clearClipboardActors()
{
    clearClipboardUnits();
    clearClipboardSprites();
}

void ClipboardAnimations::updateClipboardOwners(u8 newOwner)
{
    for ( auto & actor : unitActors )
    {
        for ( auto usedImageIndex : actor.usedImages )
        {
            if ( usedImageIndex != 0 )
                images[usedImageIndex]->owner = newOwner;
        }
    }

    for ( auto & actor : spriteActors )
    {
        for ( auto usedImageIndex : actor.usedImages )
        {
            if ( usedImageIndex != 0 )
                images[usedImageIndex]->owner = newOwner;
        }
    }
}

void ClipboardAnimations::cleanDrawList()
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
                    unitActors[index].drawListIndex = i;
                else
                    spriteActors[index].drawListIndex = i;
            }
        }
        drawListDirty = false;
    }
}

void ClipboardAnimations::animate(uint64_t currentTick)
{
    for ( auto & unitActor : unitActors )
        unitActor.animate(currentTick, true, *this);

    for ( auto & spriteActor : spriteActors )
        spriteActor.animate(currentTick, false, *this);

    cleanDrawList();
}
