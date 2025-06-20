#include <mapping_core/map_actor.h>
#include "../chkdraft.h" // TODO: recheck whether there's a way this file could go in mapping_core too

u16 & MapActor::getNewImageSlot(bool above, MapImage & image, MapAnimations & animations)
{
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
                        return usedImages[i+1];
                    }
                    else // Draw before image/use slot before image
                    {
                        std::rotate(&usedImages[i], &usedImages[nextAvailable], &usedImages[nextAvailable+1]); // Move available slot before images slot
                        return usedImages[i];
                    }
                }
            }
            return usedImages[0]; // No slot was available
        }
    }
    logger.error("Image was not present in usedImages and/or the image list");
    return usedImages[0];
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

void MapActor::animate(std::uint64_t currentTick, bool isUnit, MapAnimations & animations)
{
    for ( u16 i=0; i<u16(std::size(usedImages)) && usedImages[std::size_t(i)] != 0; ++i )
        animations.images[std::size_t(usedImages[i])]->animate(currentTick, animations, *this, isUnit);
}
