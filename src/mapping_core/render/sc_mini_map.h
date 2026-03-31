#ifndef SCMINIMAP_H
#define SCMINIMAP_H
#include <mapping_core/mapping_core.h>
#include <cstdint>
#include <vector>

struct ClassicMiniMap
{
    const Sc::Data & scData;
    const Scenario & map; // Reference to the map this instance of graphics renders

    struct Options
    {
        bool scrPreview = false; // Terrain and no units except for start locations, other unit rendering settings are ignored if set
        bool drawStartLocations = true;
        bool drawMapRevealers = true;
        bool drawSpriteUnits = true; // Overrides the setting for draw doodad sprite units
        bool drawDoodadSpriteUnits = true; // Draws doodad-specific sprite units
    };
    Options opts {};
    std::size_t width = 128;
    std::size_t height = 128;
    std::vector<std::uint32_t> pixels;

    ClassicMiniMap(const Sc::Data & scData, const Scenario & map);

    void render();
    void renderTerrain();
    void renderUnits();
    void renderFog();

private:
    float miniMapScale();
};

#endif