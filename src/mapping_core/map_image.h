#ifndef MAPIMAGE_H
#define MAPIMAGE_H
#include "sc.h"

class MapActor;
class AnimContext;

struct MapImage
{
    enum class DrawFunction : u8 {
        Normal = 0,
        Cloak = 5,
        Cloaked = 6,
        Decloak = 7,
        Remap = 9,
        Shadow = 10,
        Selection = 13,
        Hallucination = 16,
        None
    };
    static constexpr std::string_view drawFunctionNames[] {
        "0: Normal",
        "1: -",
        "2: -",
        "3: -",
        "4: -",
        "5: -",
        "6: Cloaked",
        "7: -",
        "8: -",
        "9: Remap",
        "10: Shadow",
        "11: -",
        "12: -",
        "13: Selection",
        "14: -",
        "15: -",
        "16: Hallucination",
        "17: None"
    };
    
    u16 imageId = 0;
    u8 owner = 0;
    u8 direction = 0;
    s32 xc = 0;
    s32 yc = 0;
    s8 xOffset = 0;
    s8 yOffset = 0;
    u8 remapping = 0;
    u8 selColor = 0;
    u16 baseFrame = 0;
    u16 frame = 0;
    bool flipped = false;
    bool rotation = false;
    bool hiddenLeader = false;
    bool drawIfCloaked = false;
    bool hidden = false;
    DrawFunction drawFunction = DrawFunction::Normal;

    u16 returnOffset = 0;
    u32 iScriptId = 0;
    const Sc::Sprite::IScriptAnimation* animation = nullptr;
    std::uint64_t waitUntil {};

    void error(std::string_view message);
    bool end();
    bool hasEnded() const;
    void playFrame(std::size_t frame);
    void setDirection(u8 direction);
    void advanceBy(size_t numBytes);
};

struct ActorContext
{
    std::uint64_t currentTick;
    AnimContext & animations;
    MapActor & actor;
    bool isUnit;
};

struct Animator
{
    ActorContext & context;
    std::size_t currImageIndex; // Does not change
    MapImage* currImage; // May be updated when new images are added
    bool unbreak = false; // If true, animation will skip waits till reaching nobrkcodeend

    void setActorDirection(u8 direction);

    void initializeImage(size_t iScriptId);
    void endImage();

    void createOverlay(u16 imageId, s8 x, s8 y, bool above);
    void createSpriteOverlay(u16 spriteId, s8 x, s8 y, bool above);

    void animate();
};

#endif