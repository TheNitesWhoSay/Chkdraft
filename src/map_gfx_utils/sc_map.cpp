#include "sc_map.h"

void ScMap::initAnims() // Initializes the animations for all the units and sprites in the map
{
    if ( Scenario::numUnits() > 0 || Scenario::numSprites() > 0 )
    {
        for ( std::size_t i=0; i<Scenario::numUnits(); ++i )
            animations.addUnit(i, Scenario::view.units.attached_data(i));

        for ( std::size_t i=0; i<Scenario::numSprites(); ++i )
            animations.addSprite(i, Scenario::view.sprites.attached_data(i));
    }

    this->graphics.windowBoundsChanged({
        .left = 0, .top = 0, .right = 800, .bottom = 600
    });
}

// Load map at filePath
ScMap::ScMap(Sc::Data & scData, const std::string & filePath) :
    MapFile(filePath),
    animations(scData, gameClock, static_cast<const Scenario &>(*this)),
    graphics(scData, static_cast<Scenario &>(*this), animations) { initAnims(); }

// Load map selected from browser
ScMap::ScMap(Sc::Data & scData, FileBrowserPtr<SaveType> fileBrowser) :
    MapFile(fileBrowser),
    animations(scData, gameClock, static_cast<const Scenario &>(*this)),
    graphics(scData, static_cast<Scenario &>(*this), animations) { initAnims(); }

// Create a new map
ScMap::ScMap(Sc::Data & scData, Sc::Terrain::Tileset tileset, u16 width, u16 height, size_t terrainTypeIndex,
    SaveType saveType, const Sc::Terrain::Tiles* tilesetData) :
    MapFile(tileset, width, height, terrainTypeIndex, Chk::DefaultTriggers::NoTriggers, saveType, tilesetData),
    animations(scData, gameClock, static_cast<const Scenario &>(*this)),
    graphics(scData, static_cast<Scenario &>(*this), animations) { initAnims(); }

// Creates an uninitialized map that must be manually loaded before use
ScMap::ScMap(Sc::Data & scData) : MapFile(),
    animations(scData, gameClock, static_cast<const Scenario &>(*this)),
    graphics(scData, static_cast<Scenario &>(*this), animations) { initAnims(); }

ScMap::Simulation ScMap::simulateAnim(std::uint64_t ticks)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto startTick = gameClock.currentTick();
    for ( auto i=startTick; i<startTick+ticks; ++i )
    {
        gameClock.setTickCount(i);
        animations.animate(i);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    return Simulation {
        .ticks = int(ticks),
        .gameTimeSimulatedMs = 42*int(ticks),
        .realTimeSpentMs = int(std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count())
    };
}
