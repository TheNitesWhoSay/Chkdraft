#ifndef CLIPBOARDANIMATIONS_H
#define CLIPBOARDANIMATIONS_H
#include <mapping_core/map_actor.h>
#include <mapping_core/sc.h>
#include <vector>
#include "map_animations.h"

class Clipboard;

class ClipboardAnimations : public AnimContext
{
    std::vector<MapActor> unitActors {};
    std::vector<MapActor> spriteActors {};

protected:
    Clipboard & clipboard;

public:
    ClipboardAnimations(const Sc::Data & scData, const GameClock & gameClock, Clipboard & clipboard);

    const std::vector<MapActor> & getUnitActors();
    const std::vector<MapActor> & getSpriteActors();

    void initClipboardUnits();
    void initClipboardSprites();
    void clearClipboardUnits();
    void clearClipboardSprites();
    void clearClipboardActors();
    void updateClipboardOwners(u8 newOwner);

    void cleanDrawList();
    void animate(uint64_t currentTick);
};

#endif