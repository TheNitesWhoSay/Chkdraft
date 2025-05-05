#ifndef MAPANIMATIONS_H
#define MAPANIMATIONS_H
#include "mapping/map_actor.h"
#include "mapping/map_image.h"
#include "mapping_core/mapping_core.h"

struct MapAnimations
{
    std::vector<MapActor> unitActors; // animatable units, parallel array to Scenario::units
    std::vector<MapActor> spriteActors; // animatable sprites, parallel array to Scenario::sprites
    std::vector<MapImage> images; // contains the images associated with unitActors and spriteActors

    MapAnimations();

    void initialize(const Scenario & scenario);

    void animate(uint64_t currentTick);
};

#endif