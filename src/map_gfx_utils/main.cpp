#include "gfx_util.h"
#include <iostream>

extern Logger logger;

/** If a map is coming not from the filesystem but from something that can be loaded in memory... Or if using a parser more robust than chkd...
    This serves as an example of how to place such data into a map for rendering

    All the Chk:: data structures are packed so you should be able to allocate a vector then push binary data into it if desired
*/
std::unique_ptr<ScMap> exampleLoadFromMemory(GfxUtil & gfxUtil, Sc::Terrain::Tileset tileset)
{
    auto scMap = gfxUtil.createBlankMap();

    std::vector<Chk::Unit> units {};
    std::vector<Chk::Sprite> sprites {};
    for ( std::size_t i=0; i<Sc::Unit::TotalTypes; ++i )
    {
        int x = i%32;
        int y = i/32;
        units.push_back(Chk::Unit {0, u16(x*64+64), u16(y*64+64), Sc::Unit::Type(i), 0, 0, 0, Sc::Player::Id::Player1});
    }
    for ( std::size_t i=0; i<Sc::Sprite::TotalSprites; ++i )
    {
        int x = i%32;
        int y = i/32;
        //sprites.push_back(Chk::Sprite {Sc::Sprite::Type(i), u16(x*64+64), u16(y*64+64), Sc::Player::Id::Player1, 0, Chk::Sprite::SpriteFlags::DrawAsSprite});
    }

    std::vector<u16> mtxmTerrain(std::size_t(64*64), u16(0));
    std::iota(mtxmTerrain.begin(), mtxmTerrain.end(), 0); // Fills with ascending tile ids starting from 0

    scMap->initData(MapData {
        .sprites = std::move(sprites),
        .units = std::move(units),
        .dimensions { .tileWidth = 64, .tileHeight = 64 },
        .tileset = tileset,
        .tiles = std::move(mtxmTerrain),
    });
    scMap->initAnims();
    
    return scMap;
}

void testRender(GfxUtil & gfxUtil, Renderer & renderer, Sc::Terrain::Tileset tileset, const std::string & outFilePath)
{
    Renderer::Options options {
        .drawStars = true,
        .drawTerrain = true,
        .drawActors = true,
        .drawFogPlayer = std::nullopt,
        .drawLocations = false,
        .displayFps = false
    };

    auto mapStartTime = std::chrono::high_resolution_clock::now();
    //auto map = gfxUtil.loadMap(); // Use a browser
    auto map = gfxUtil.loadMap("C:/misc/bacfffa368f90a640919badc88155a577dd3c67d2ea4ba1fe65d9e1983edd5fb.scx"); // Use an absolute file path
    //auto map = exampleLoadFromMemory(gfxUtil, tileset); // Use something you already have in memory
    auto mapLoadTime = std::chrono::high_resolution_clock::now();

    auto animTime = map->simulateAnim(52); // Simulate n anim ticks occuring, need at least 52 to extend all the tanks

    //renderer.displayInGui(*map, options, true);
    auto imageTimes = renderer.saveMapImageAsWebP(*map, options, outFilePath);
    auto mapFinishTime = std::chrono::high_resolution_clock::now();
    logger.log(GfxUtilInfo) << MapTimings(mapStartTime, mapLoadTime, animTime, imageTimes, mapFinishTime) << std::endl;
}

int main()
{
    try {
        auto startInitialLoad = std::chrono::high_resolution_clock::now();
        GfxUtil gfxUtil {};
        gfxUtil.loadScData(); // Could provide a path string to this method
        auto renderer = gfxUtil.createRenderer(RenderSkin::Classic);
        auto endInitialLoad = std::chrono::high_resolution_clock::now();

        logger.log(GfxUtilInfo) << "Initial load completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(endInitialLoad-startInitialLoad).count() << "ms" << std::endl;

        testRender(gfxUtil, *renderer, Sc::Terrain::Tileset::SpacePlatform, getDefaultFolder() + "space.webp");
        //testRender(gfxUtil, *renderer, Sc::Terrain::Tileset::Jungle, getDefaultFolder() + "jungle.webp");
        //testRender(gfxUtil, *renderer, Sc::Terrain::Tileset::SpacePlatform, getDefaultFolder() + "space2.webp");
    } catch ( std::exception & e ) {
        logger.fatal("Unhandled exception: ", e);
        throw;
    } catch ( ... ) {
        logger.fatal("Unknown unhandled exception");
        throw;
    }
    return 0;
}
