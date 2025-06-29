#include <mapping_core/map_actor.h>
#include "../chkdraft.h" // TODO: recheck whether there's a way this file could go in mapping_core too

u16 & MapActor::getNewImageSlot(bool above, MapImage & image, MapAnimations & animations)
{
    u16 primaryImageImageIndex = usedImages[primaryImageIndex];
    auto updatePrimaryImageIndex = [&]() {
        for ( std::size_t j=0; j<MaxSlots; ++j )
        {
            if ( usedImages[j] == primaryImageImageIndex )
                primaryImageIndex = j;
        }
    };
    defragmentNonZeroes(usedImages);
    for ( std::size_t i=0; i<MaxSlots; ++i )
    {
        if ( usedImages[i] != 0 && usedImages[i] < animations.images.size() && // If i is the current images slot
            animations.images[usedImages[i]].has_value() && (&image == &(animations.images[usedImages[i]].value())) )
        {
            for ( std::size_t nextAvailable=i+1; nextAvailable<MaxSlots; ++nextAvailable )
            {
                if ( usedImages[nextAvailable] == 0 ) // This slot is available
                {
                    if ( above ) // Draw after image/use slot after image
                    {
                        std::rotate(&usedImages[i+1], &usedImages[nextAvailable], &usedImages[nextAvailable+1]); // Move available slot after images slot
                        updatePrimaryImageIndex();
                        return usedImages[i+1];
                    }
                    else // Draw before image/use slot before image
                    {
                        std::rotate(&usedImages[i], &usedImages[nextAvailable], &usedImages[nextAvailable+1]); // Move available slot before images slot
                        updatePrimaryImageIndex();
                        return usedImages[i];
                    }
                }
            }
            updatePrimaryImageIndex();
            return usedImages[0]; // No slot was available
        }
    }
    logger.error("Image was not present in usedImages and/or the image list");
    updatePrimaryImageIndex();
    return usedImages[0];
}

void MapActor::removeImage(std::size_t imageIndex)
{
    u16 primaryImageImageIndex = usedImages[primaryImageIndex];
    auto updatePrimaryImageIndex = [&]() {
        for ( std::size_t j=0; j<MaxSlots; ++j )
        {
            if ( usedImages[j] == primaryImageImageIndex )
                primaryImageIndex = j;
        }
    };
    for ( auto & usedImage : usedImages )
    {
        if ( usedImage == imageIndex )
            usedImage = 0;
    }
    defragmentNonZeroes(usedImages);
    primaryImageIndex = 0;
    updatePrimaryImageIndex();
}

MapImage* MapActor::primaryImage(MapAnimations & animations)
{
    if ( primaryImageIndex > std::size(usedImages) )
    {
        logger.error() << "Invalid primaryImageIndex: " << primaryImageIndex << '\n';
        return nullptr;
    }
    else if ( usedImages[primaryImageIndex] == 0 || usedImages[primaryImageIndex] >= animations.images.size() ||
        animations.images[usedImages[primaryImageIndex]] == std::nullopt )
    {
        logger.error() << "Invalid imageIndex: " << usedImages[primaryImageIndex] << '\n';
        return nullptr;
    }
    else
        return &(animations.images[usedImages[primaryImageIndex]].value());
}

void MapActor::setAnim(Sc::Sprite::AnimHeader animHeader, std::uint64_t currentTick, bool isUnit, MapAnimations & animations)
{
    for ( int i=0; noBreakSection; ++i )
    {
        animate(currentTick, isUnit, animations, true);
        if ( i == 100 ) // No apparent noBreakSection end
        {
            AnimationContext context { .currentTick = currentTick, .animations = animations, .actor = *this, .isUnit = isUnit };
            animations.restartActor(context); // Try restarting the actor
            for ( int j=0; noBreakSection; ++j )
            {
                animate(currentTick, isUnit, animations, true);
                if ( j == 100 ) // No apparent noBreakSection end
                {
                    logger.error("Could not set anim, stuck in unrecoverable noBreakSection state");
                    return;
                }
            }
            break;
        }
    }

    for ( std::ptrdiff_t i=std::size(usedImages)-1; i>=0 ; --i )
    {
        if ( usedImages[i] != 0 )
        {
            MapImage* image = &animations.images[std::size_t(usedImages[i])].value();
            const Sc::Sprite::IScriptAnimation* anim = chkd.scData.sprites.getAnimationHeader(image->iScriptId, animHeader);
            if ( anim == nullptr )
            {
                if ( i == primaryImageIndex )
                    logger.error("Could not set anim, header not found");

                continue;
            }

            image->animation = anim;
            image->waitUntil = currentTick;
            animate(currentTick, isUnit, animations, false);
        }
    }
}

void MapActor::animate(std::uint64_t currentTick, bool isUnit, MapAnimations & animations, bool unbreak)
{
    AnimationContext context {
        .currentTick = currentTick,
        .animations = animations,
        .actor = *this,
        .isUnit = isUnit,
    };
    bool allEnded = true;
    for ( std::ptrdiff_t i=std::size(usedImages)-1; i>=0 ; --i )
    {
        if ( usedImages[i] != 0 )
        {
            MapImage* image = &animations.images[std::size_t(usedImages[i])].value();
            if ( image->hasEnded() )
            {
                if ( autoRestart )
                    image->drawFunction = MapImage::DrawFunction::None;
            }
            else
            {
                if ( !image->hiddenLeader )
                    allEnded = false;

                if ( unbreak )
                    image->waitUntil = currentTick;

                if ( image->yOffset != transitShadowTarget && image->drawFunction == MapImage::DrawFunction::Shadow )
                {
                    if ( image->yOffset < transitShadowTarget )
                        image->yOffset += 2;
                    else if ( image->yOffset > transitShadowTarget )
                        image->yOffset -= 1;
                }

                Animator {
                    .context = context,
                    .currImageIndex = usedImages[i],
                    .currImage = image,
                    .unbreak = unbreak
                }.animate();
            }
        }
    }
    if ( allEnded && autoRestart )
        animations.restartActor(context);
}
