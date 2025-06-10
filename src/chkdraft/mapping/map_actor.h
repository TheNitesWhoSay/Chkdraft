#ifndef MAPACTOR_H
#define MAPACTOR_H
#include <mapping/map_image.h>
#include <mapping_core/sc.h>
#include <cstdint>
#include <string_view>
#include <vector>

class MapAnimations;

// An extension to a Chk::Unit or Chk::Sprite focusing on animation and linking up with associated images
struct MapActor
{
    u16 returnOffset = 0;
    size_t iScriptId = 0;
    const Sc::Sprite::IScriptAnimation* animation = nullptr;
    s8 xOffset = 0;
    s8 yOffset = 0;
    u8 direction = 0;
    std::uint64_t waitUntil {};
    u16 usedImages[10] {};

    u16 & nextImageSlot(); // If result is non-zero, no image slots are available
    
    void initialize(std::uint64_t currentTick, size_t iScriptId, bool isUnit, MapAnimations & animations);
    bool end();
    void error(std::string_view message);
    void restartIfEnded(std::uint64_t currentTick, bool isUnit, MapAnimations & animations);

    void advanceBy(size_t numBytes);
    void animate(std::uint64_t currentTick, bool isUnit, MapAnimations & animations);

    void setDirection(MapImage & image, u8 direction);
    void createOverlay(u16 imageId, s8 x, s8 y, MapAnimations & animations);
};

#endif