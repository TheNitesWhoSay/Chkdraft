#include "gfx_util.h"
#include <cross_cut/logger.h>

Logger logger {};

std::string getDefaultFolder()
{
#ifdef WIN32
    return "c:/misc/";
#else
    return std::string(getenv("HOME")) + "/";
#endif
}

void GfxUtil::loadScData(const std::string & expectedScPath)
{
    if ( scData != nullptr )
        throw std::logic_error("loadScData called twice");

    bool isRemastered = false;
    bool declinedBrowse = false;

    Sc::DataFile::Browser scBrowser {};
    bool foundStarCraftDirectory = Sc::DataFile::Browser{}.findStarCraftDirectory(this->starCraftDirectory, isRemastered, declinedBrowse,
        expectedScPath, Sc::DataFile::Browser::getDefaultStarCraftBrowser());

    if ( !foundStarCraftDirectory )
    {
        logger.error("Failed to load data from StarCraft");
        throw std::runtime_error("Failed to load data from StarCraft");
    }

    auto loadScData = std::make_unique<Sc::Data>();
    if ( loadScData->load(std::make_shared<Sc::DataFile::Browser>(), Sc::DataFile::getDefaultDataFiles(), this->starCraftDirectory, nullptr) )
        std::swap(this->scData, loadScData);
}

std::unique_ptr<Renderer> GfxUtil::createRenderer(RenderSkin skin, ScMap & map)
{
    if ( !scData )
    {
        logger.error("scData should be loaded prior to creating a renderer");
        return nullptr;
    }
    auto renderer = std::make_unique<Renderer>(*this->scData);
    renderer->starCraftDirectory = this->starCraftDirectory;
    renderer->initialize();
    renderer->loadSkinAndTileSet(skin, map);
    return renderer;
}

// Providing a map to createRenderer would prevent loading two tilesets prior to the first render
// This doesn't matter in batch but if running just for one map use the other overload
std::unique_ptr<Renderer> GfxUtil::createRenderer(RenderSkin skin)
{
    auto dummyMap = createMap(Sc::Terrain::Tileset::Badlands, 64, 64);
    return createRenderer(skin, *dummyMap);
}

// Load map selected from browser
std::unique_ptr<ScMap> GfxUtil::loadMap()
{
    if ( !scData )
    {
        logger.error("scData should be loaded prior to loading a map");
        return nullptr;
    }

    try {
        auto result = std::make_unique<ScMap>(*scData, MapFile::getDefaultOpenMapBrowser());
        return result;
    } catch ( std::exception & e ) {
        logger.error(e.what());
        return nullptr;
    }
}

// Load map at filePath
std::unique_ptr<ScMap> GfxUtil::loadMap(const std::string & mapFilePath)
{
    if ( !scData )
    {
        logger.error("scData should be loaded prior to loading a map");
        return nullptr;
    }

    try {
        auto result = std::make_unique<ScMap>(*scData, mapFilePath);
        return result;
    } catch ( std::exception & e ) {
        logger.error(e.what());
        return nullptr;
    }
}

// Create a new map
std::unique_ptr<ScMap> GfxUtil::createMap(Sc::Terrain::Tileset tileset, u16 width, u16 height, size_t terrainTypeIndex, SaveType saveType)
{
    if ( !scData )
    {
        logger.error("scData should be loaded prior to creating a map");
        return nullptr;
    }

    try {
        if ( terrainTypeIndex == std::numeric_limits<size_t>::max() )
        {
            switch ( Sc::Terrain::Tileset(size_t(tileset) % Sc::Terrain::NumTilesets) )
            {
            case Sc::Terrain::Tileset::Badlands: terrainTypeIndex = Sc::Isom::Brush::Badlands::Default; break;
            case Sc::Terrain::Tileset::SpacePlatform: terrainTypeIndex = Sc::Isom::Brush::Space::Default; break;
            case Sc::Terrain::Tileset::Installation: terrainTypeIndex = Sc::Isom::Brush::Installation::Default; break;
            case Sc::Terrain::Tileset::Ashworld: terrainTypeIndex = Sc::Isom::Brush::Ashworld::Default; break;
            case Sc::Terrain::Tileset::Jungle: terrainTypeIndex = Sc::Isom::Brush::Jungle::Default; break;
            case Sc::Terrain::Tileset::Desert: terrainTypeIndex = Sc::Isom::Brush::Desert::Default; break;
            case Sc::Terrain::Tileset::Arctic: terrainTypeIndex = Sc::Isom::Brush::Arctic::Default; break;
            case Sc::Terrain::Tileset::Twilight: terrainTypeIndex = Sc::Isom::Brush::Twilight::Default; break;
            }
        }
        auto result = std::make_unique<ScMap>(*scData, tileset, width, height, terrainTypeIndex, saveType, &(scData->terrain.get(tileset)));
        return result;
    } catch ( std::exception & e ) {
        logger.error(e.what());
        return nullptr;
    }
}

std::unique_ptr<ScMap> GfxUtil::createBlankMap()
{
    if ( !scData )
    {
        logger.error("scData should be loaded prior to creating a map");
        return nullptr;
    }

    try {
        auto result = std::make_unique<ScMap>(*scData);
        return result;
    } catch ( std::exception & e ) {
        logger.error(e.what());
        return nullptr;
    }
}

// Creates an uninitialized map that must be manually loaded before use
std::unique_ptr<ScMap> GfxUtil::createUninitializedMap()
{
    if ( !scData )
    {
        logger.error("scData should be loaded prior to creating a map");
        return nullptr;
    }

    try {
        auto result = std::make_unique<ScMap>(*scData);
        return result;
    } catch ( std::exception & e ) {
        logger.error(e.what());
        return nullptr;
    }
}
