#ifndef MAPACTOR_H
#define MAPACTOR_H
#include <mapping/map_image.h>
#include <mapping_core/sc.h>
#include <cstdint>
#include <string_view>
#include <vector>

// An extension to a Chk::Unit or Chk::Sprite focusing on animation and linking up with associated images
struct MapActor
{
    std::vector<std::optional<MapImage>> & mapImages;
    u16 returnOffset = 0;
    size_t iScriptId = 0;
    //size_t frame = 0;
    const Sc::Sprite::IScriptAnimation* animation = nullptr;
    // TODO: More fields such as a flipped indicator, offsets and whatnot
    //s32 xOffset = 0;
    //s32 yOffset = 0;
    u8 direction = 0;
    std::uint64_t waitUntil {};
    u16 usedImages[10] {};

    MapActor & operator=(const MapActor & other);
    
    void initialize(std::uint64_t currentTick, size_t iScriptId, bool isUnit);
    bool end();
    void error(std::string_view message);
    void restartIfEnded(std::uint64_t currentTick, bool isUnit);

    void advanceBy(size_t numBytes);
    void animate(std::uint64_t currentTick, bool isUnit);

    void setDirection(u8 direction);
};

#endif