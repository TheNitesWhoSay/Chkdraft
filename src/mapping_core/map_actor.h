#ifndef MAPACTOR_H
#define MAPACTOR_H
#include <cstdint>
#include <string_view>
#include <vector>
#include "map_image.h"
#include "sc.h"

class AnimContext;

// An extension to a Chk::Unit or Chk::Sprite focusing on animation and linking up with associated images
struct MapActor
{
    static constexpr std::size_t MaxSlots = 10;

    s8 transitShadowTarget = 0;
    u8 lastSetAnim = u8(Sc::Sprite::AnimHeader::Init);
    bool noBreakSection = false;
    bool autoRestart = false;
    bool paused = false;
    u8 direction = 0;
    u8 elevation = 0;
    u16 drawListIndex = 0;
    u16 primaryImageSlot = 0;
    u16 usedImages[MaxSlots] {}; // Only leftmost indexes are used

    u16 & getNewImageSlot(bool above, MapImage & image, AnimContext & animations); // If result is non-zero, no image slots are available
    void removeImage(std::size_t imageIndex);

    MapImage* primaryImage(AnimContext & animations);

    void setAnim(Sc::Sprite::AnimHeader animHeader, std::uint64_t currentTick, bool isUnit, AnimContext & animations, bool silent = false);
    void setDrawFunction(MapImage::DrawFunction drawFunc, AnimContext & animations);
    void hideNonCloakImages(AnimContext & animations);
    void showNonCloakImages(AnimContext & animations);
    void animate(std::uint64_t currentTick, bool isUnit, AnimContext & animations, bool unbreak = false);
};

#endif