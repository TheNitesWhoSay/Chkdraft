#ifndef SCMAP_H
#define SCMAP_H
#include <mapping_core/map_file.h>
#include <mapping_core/render/game_clock.h>
#include <mapping_core/render/map_animations.h>
#include <mapping_core/render/sc_gl_graphics.h>

// A StarCraft map including the MapFile (scx/scm/chk), a game clock, animations, and graphics
struct ScMap : MapFile
{
    void initAnims(); // Initializes the animations for all the units and sprites in the map

    GameClock gameClock {};
    MapAnimations animations;
    MapGraphics graphics;

    // Load map at filePath
    ScMap(Sc::Data & scData, const std::string & filePath);

    // Load map selected from browser
    ScMap(Sc::Data & scData, FileBrowserPtr<SaveType> fileBrowser /*= MapFile::getDefaultOpenMapBrowser()*/);

    // Create a new map
    ScMap(Sc::Data & scData, Sc::Terrain::Tileset tileset, u16 width = 64, u16 height = 64, size_t terrainTypeIndex = 0,
        SaveType saveType = SaveType::HybridScm, const Sc::Terrain::Tiles* tilesetData = nullptr);

    // Creates an uninitialized map that must be manually loaded before use
    ScMap(Sc::Data & scData);

    struct Simulation {
        int ticks = 0;
        int gameTimeSimulatedMs = 0;
        int realTimeSpentMs = 0;
    };
    Simulation simulateAnim(std::uint64_t ticks);
};

#endif