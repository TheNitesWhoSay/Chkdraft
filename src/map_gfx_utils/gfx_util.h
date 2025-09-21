#ifndef GFXUTIL_H
#define GFXUTIL_H
#include "map_timings.h"
#include "renderer.h"
#include "screenshot.h"
#include <cross_cut/logger.h>
#include <mapping_core/mapping_core.h>
#include <mapping_core/render/map_animations.h>
#include <mapping_core/render/sc_gl_graphics.h>

inline LogLevel GfxUtilInfo = LogLevel::Info+1; // 1 above the regular LogLevel::Info s.t. mapping core info logs don't need to be included

std::string getDefaultFolder();

struct GfxUtil
{
    std::unique_ptr<Sc::Data> scData = nullptr;
    std::string starCraftDirectory {};

    void loadScData(const std::string & expectedScPath = ::getDefaultScPath());

    std::unique_ptr<Renderer> createRenderer(RenderSkin skin, ScMap & map);

    // Providing a map to createRenderer would prevent loading two tilesets prior to the first render
    // This doesn't matter in batch but if running just for one map use the other overload
    std::unique_ptr<Renderer> createRenderer(RenderSkin skin);

    // Load map selected from browser
    std::unique_ptr<ScMap> loadMap();

    // Load map at filePath
    std::unique_ptr<ScMap> loadMap(const std::string & mapFilePath);

    // Create a new map
    std::unique_ptr<ScMap> createMap(Sc::Terrain::Tileset tileset, u16 width = 64, u16 height = 64,
        size_t terrainTypeIndex = std::numeric_limits<size_t>::max(), SaveType saveType = SaveType::HybridScm);

    std::unique_ptr<ScMap> createBlankMap();

    // Creates an uninitialized map that must be manually loaded before use
    std::unique_ptr<ScMap> createUninitializedMap();
};

#endif