#ifndef ANIMATION_H
#define ANIMATION_H
#include <mapping_core/sc.h>
#include <cstdint>
#include <string_view>

struct MapActor {
    u16 returnOffset = 0;
    size_t iScriptId = 0;
    size_t frame = 0;
    const Sc::Sprite::IScriptAnimation* animation = nullptr;
    // TODO: More fields such as a flipped indicator, offsets and whatnot
    s32 xOffset = 0;
    s32 yOffset = 0;
    std::uint64_t waitUntil {};
    
    void initialize(std::uint64_t currentTick, size_t iScriptId);
    bool end();
    void error(std::string_view message);
    void restartIfEnded(std::uint64_t currentTick);

    void advanceBy(size_t numBytes);
    void animate(std::uint64_t currentTick);
};

#endif