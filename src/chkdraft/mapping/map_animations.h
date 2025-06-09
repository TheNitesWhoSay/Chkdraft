#ifndef MAPANIMATIONS_H
#define MAPANIMATIONS_H
#include "mapping/map_actor.h"
#include "mapping/map_image.h"
#include "mapping_core/mapping_core.h"
#include <optional>

class MapAnimations
{
    const Scenario & scenario;
    std::vector<u16> availableImages {};

    u16 createImage();
    void removeImage(u16 imageIndex);

public:
    std::vector<MapActor> unitActors; // animatable units, parallel array to Scenario::units
    std::vector<MapActor> spriteActors; // animatable sprites, parallel array to Scenario::sprites
    std::vector<std::optional<MapImage>> images; // contains the images associated with unitActors and spriteActors

    MapAnimations(const Scenario & scenario);

    void initialize();
    void addUnit(std::size_t unitIndex);
    void addSprite(std::size_t spriteIndex);
    void removeUnit(std::size_t unitIndex);
    void removeSprite(std::size_t spriteIndex);

    void animate(uint64_t currentTick);
};

#endif