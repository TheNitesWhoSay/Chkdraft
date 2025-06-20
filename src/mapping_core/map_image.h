#ifndef MAPIMAGE_H
#define MAPIMAGE_H
#include "sc.h"

class MapActor;
class MapAnimations;

struct MapImage
{
    enum class DrawFunction : u8 {
        Normal = 0,
        Shadow = 10,
    };
    
    u16 imageId = 0;
    u8 owner = 0;
    u8 direction = 0;
    s32 xc = 0;
    s32 yc = 0;
    s8 xOffset = 0;
    s8 yOffset = 0;
    size_t baseFrame = 0;
    size_t frame = 0;
    bool flipped = false;
    DrawFunction drawFunction = DrawFunction::Normal;

    u16 returnOffset = 0;
    size_t iScriptId = 0;
    const Sc::Sprite::IScriptAnimation* animation = nullptr;
    std::uint64_t waitUntil {};

    void error(std::string_view message);

    void initialize(std::uint64_t currentTick, size_t iScriptId, MapAnimations & animations, MapActor & actor, bool isUnit);
    void restartIfEnded(std::uint64_t currentTick, MapAnimations & animations, MapActor & actor, bool isUnit);
    bool end();

    void playFrame(u8 frame);
    void setDirection(u8 direction);
    void createOverlay(u16 imageId, s8 x, s8 y, MapAnimations & animations, MapActor & actor, bool isUnit, bool above);

    void advanceBy(size_t numBytes);
    void animate(std::uint64_t currentTick, MapAnimations & animations, MapActor & actor, bool isUnit);
};

#endif