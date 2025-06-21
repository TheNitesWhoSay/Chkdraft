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
    bool rotation = false;
    DrawFunction drawFunction = DrawFunction::Normal;

    u16 returnOffset = 0;
    size_t iScriptId = 0;
    const Sc::Sprite::IScriptAnimation* animation = nullptr;
    std::uint64_t waitUntil {};

    void error(std::string_view message);
    bool end();
    void playFrame(u8 frame);
    void setDirection(u8 direction);
    void advanceBy(size_t numBytes);
};

struct AnimationContext
{
    std::uint64_t currentTick;
    MapAnimations & animations;
    MapActor & actor;
    bool isUnit;
};

struct Animator
{
    AnimationContext & context;
    std::size_t currImageIndex; // Does not change
    MapImage* currImage; // May be updated when new images are added

    void setActorDirection(u8 direction);

    void initializeImage(size_t iScriptId);

    void restartIfEnded();

    void createOverlay(u16 imageId, s8 x, s8 y, bool above);

    void animate();
};

#endif