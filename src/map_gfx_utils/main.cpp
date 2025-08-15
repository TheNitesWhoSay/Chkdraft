#include "screenshot.h"
#include <glfw/context.h>
#include <glfw/window.h>
#include <glad/glad.h>
#include <glad/utils.h>
#include <gl/utils.h>
#include <cross_cut/logger.h>
#include <mapping_core/mapping_core.h>
#include <mapping_core/render/map_animations.h>
#include <mapping_core/render/sc_gl_graphics.h>
#include <iostream>

void getLatoFontData(std::uint8_t const* & latoFontDataPtr, int & latoFontDataSize);

Logger logger {};

enum class RenderSkin
{
    ClassicGL,
    ScrSD,
    ScrHD2,
    ScrHD,
    CarbotHD2,
    CarbotHD,
    Total,
    Unknown,
};

// A StarCraft map including the MapFile (scx/scm/chk), a game clock, animations, and graphics
struct ScMap : MapFile
{
    void initAnims() // Initializes the animations for all the units and sprites in the map
    {
        if ( Scenario::numUnits() > 0 || Scenario::numSprites() > 0 )
        {
            for ( std::size_t i=0; i<Scenario::numUnits(); ++i )
                animations.addUnit(i, Scenario::view.units.attachedData(i));

            for ( std::size_t i=0; i<Scenario::numSprites(); ++i )
                animations.addSprite(i, Scenario::view.sprites.attachedData(i));
        }

        this->graphics.windowBoundsChanged({
            .left = 0, .top = 0, .right = 800, .bottom = 600
        });
    }

    GameClock gameClock {};
    MapAnimations animations;
    MapGraphics graphics;

    // Load map at filePath
    ScMap(Sc::Data & scData, const std::string & filePath) :
        MapFile(filePath),
        animations(scData, gameClock, static_cast<const Scenario &>(*this)),
        graphics(scData, static_cast<Scenario &>(*this), animations) { initAnims(); }

    // Load map selected from browser
    ScMap(Sc::Data & scData, FileBrowserPtr<SaveType> fileBrowser /*= MapFile::getDefaultOpenMapBrowser()*/) :
        MapFile(fileBrowser),
        animations(scData, gameClock, static_cast<const Scenario &>(*this)),
        graphics(scData, static_cast<Scenario &>(*this), animations) { initAnims(); }

    // Create a new map
    ScMap(Sc::Data & scData, Sc::Terrain::Tileset tileset, u16 width = 64, u16 height = 64, size_t terrainTypeIndex = 0, SaveType saveType = SaveType::HybridScm, const Sc::Terrain::Tiles* tilesetData = nullptr) :
        MapFile(tileset, width, height, terrainTypeIndex, saveType, tilesetData),
        animations(scData, gameClock, static_cast<const Scenario &>(*this)),
        graphics(scData, static_cast<Scenario &>(*this), animations) { initAnims(); }

    // Creates an uninitialized map that must be manually loaded before use
    ScMap(Sc::Data & scData) :
        MapFile(),
        animations(scData, gameClock, static_cast<const Scenario &>(*this)),
        graphics(scData, static_cast<Scenario &>(*this), animations) { initAnims(); }

    struct Simulation {
        int ticks = 0;
        int gameTimeSimulatedMs = 0;
        int realTimeSpentMs = 0;
    };
    Simulation simulateAnim(std::uint64_t ticks)
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
};

std::string getDefaultFolder()
{
#ifdef WIN32
    return "c:/misc/";
#else
    return std::string(getenv("HOME")) + "/";
#endif
}

/* A renderer has one set of draw settings (including the skin) active at a time,
    maps (which have chk data and an anim state) can be passed to a renderer to save screenshots or display the map;
    multiple maps may be used with the same renderer.
   
   Batching Considerations:
    - Renderer settings besides the skin can be changed without load time or additional memory use
    - Initial loads of skins is expensive, once a skin is loaded, switching skins is ~free, however
       the VRAM/RAM that the skin requires will not be free'd until the renderer is disposed of.
    - Image Textures (per-skin):
        - Initial Load: Long, happens first time using a skin
        - Memory: Collectively these take up a lot, images stay in VRAM/RAM till disposing of the renderer
    - Tileset Textures (per-skin, per-tileset):
        - Initial Load: Short, happens first time using a tileset on a given skin
        - Memory: Relatively small, tileset textures stay in VRAM/RAM till disposing of the renderer
    - If tight on memory, load a renderer for a single skin and go through a batch of maps...
       whilst on a map, vary the draw settings (except for the skin) and the anim state and get all the images you want
       then free the renderer, create a new renderer for the next skin, and go through that batch of maps for that skin */
struct Renderer
{
    glfw::Context glfwContext {};
    glfw::Window window {};
    std::string starCraftDirectory;
    const Sc::Data & scData;
    ScMap* displayedMap = nullptr;
    std::unique_ptr<GraphicsData> graphicsData;
    RenderSkin renderSkin = RenderSkin::Unknown;
    bool showGui = false;

    struct Options {
        bool drawStars = false;
        bool drawTerrain = true;
        bool drawActors = true; // Units & sprites
        std::optional<u8> drawFogPlayer = std::nullopt; // Zero-based
        bool drawLocations = false;
        bool displayFps = false;
    };

    Renderer(const Sc::Data & scData) : scData(scData) {}

    void initialize()
    {
        constexpr int initialWidth = 800;
        constexpr int initialHeight = 600;
        window.setVersionHint(3, 3);
        window.setCoreProfileHint();
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Use this to create a hidden window
        window.create(initialWidth, initialHeight, "MapGfxUtils");
        window.enableForwarding(this);

        window.makeContextCurrent();
        glad::loadGL();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        gl::enableDepthTesting();
        gl::setViewport(0, 0, initialWidth, initialHeight);

        window.setFrameBufferSizeCallback<[](GLFWwindow* window, int width, int height) { gl::setViewport(0, 0, width, height); }>();

        glViewport(0, 0, GLsizei(800), GLsizei(600));
        window.swapBuffers();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        glDisable(GL_DEPTH_TEST);

        glDisable(GL_POLYGON_SMOOTH);
        glDisable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void processInput()
    {
        if ( window.getKey(GLFW_KEY_ESCAPE) == GLFW_PRESS )
            showGui = false;
    }

    void clearWindow()
    {
        gl::clearColor(0.0f, 0.0f, 0.0f, 1.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    bool loadSkinAndTileSet(RenderSkin skin, ScMap & map)
    {
        // Validate the skin and if remastered, turn it into GraphicsData::LoadSettings
        GraphicsData::LoadSettings loadSettings {
            .visualQuality = VisualQuality::SD,
            .skinId = ::Skin::Id::Classic,
            .tileset = Sc::Terrain::Tileset(int(map->tileset) % int(Sc::Terrain::NumTilesets)),
            .forceShowStars = false
        };
        switch ( skin )
        {
            case RenderSkin::ClassicGL: loadSettings.visualQuality = VisualQuality::SD; loadSettings.skinId = ::Skin::Id::Classic; break;
            case RenderSkin::ScrSD: loadSettings.visualQuality = VisualQuality::SD; loadSettings.skinId = ::Skin::Id::Remastered; break;
            case RenderSkin::ScrHD2: loadSettings.visualQuality = VisualQuality::HD2; loadSettings.skinId = ::Skin::Id::Remastered; break;
            case RenderSkin::ScrHD: loadSettings.visualQuality = VisualQuality::HD; loadSettings.skinId = ::Skin::Id::Remastered; break;
            case RenderSkin::CarbotHD2: loadSettings.visualQuality = VisualQuality::HD2; loadSettings.skinId = ::Skin::Id::Carbot; break;
            case RenderSkin::CarbotHD: loadSettings.visualQuality = VisualQuality::HD; loadSettings.skinId = ::Skin::Id::Carbot; break;
            default: throw std::logic_error("Unrecognized skin!");
        }

        bool includesRemastered = false;
        std::shared_ptr<ArchiveCluster> archiveCluster = nullptr;
        if ( loadSettings.skinId == ::Skin::Id::Classic ) // ClassicGL relies only on the general scData loaded earlier
            archiveCluster = std::make_shared<ArchiveCluster>(std::vector<ArchiveFilePtr>{});
        else // Remastered skins usually need to load something more
            archiveCluster = Sc::DataFile::Browser{}.openScDataFiles(includesRemastered, Sc::DataFile::getDefaultDataFiles(), starCraftDirectory, nullptr);

        if ( graphicsData == nullptr )
        {
            graphicsData = std::make_unique<GraphicsData>();
            const std::uint8_t* latoFontDataPtr = nullptr;
            int latoFontDataSize = 0;
            getLatoFontData(latoFontDataPtr, latoFontDataSize);
            graphicsData->defaultFont = gl::Font::loadStatic(latoFontDataPtr, std::size_t(latoFontDataSize), 0, 12);
        }

        auto loadBuffer = ByteBuffer(1024*1024*120); // 120MB
        map.graphics.load(*graphicsData, loadSettings, *archiveCluster, loadBuffer);
        map.graphics.setFont(graphicsData->defaultFont.get());
        this->renderSkin = skin;
        return true;
    }

    void encodeMapImageAsWebP(ScMap & map, const Options & options, auto use)
    {
        int tileWidth = int(map.getTileWidth());
        int tileHeight = int(map.getTileHeight());
        int maxTileDimension = std::max(tileWidth, tileHeight);
        int zoomSizeMultiplier = 1;
        switch ( renderSkin )
        {
            case RenderSkin::ClassicGL: zoomSizeMultiplier = 1; break;
            case RenderSkin::ScrSD: zoomSizeMultiplier = 1; break;
            case RenderSkin::ScrHD2: zoomSizeMultiplier = 2; break;
            case RenderSkin::ScrHD: zoomSizeMultiplier = 4; break;
            case RenderSkin::CarbotHD2: zoomSizeMultiplier = 2; break;
            case RenderSkin::CarbotHD: zoomSizeMultiplier = 4; break;
            case RenderSkin::Unknown: case RenderSkin::Total:
                throw std::logic_error("encodeMapImageAsWebP called without properly initializing the renderer skin!");
        }

        // HD and HD2 are best 400% and 200% zoomed respectively, and can be smoothly halved until they fit in one image
        int pixelSizeMultiplier = 32 * zoomSizeMultiplier;
        while ( zoomSizeMultiplier > 1 && pixelSizeMultiplier*maxTileDimension > 16384 )
        {
            zoomSizeMultiplier /= 2;
            pixelSizeMultiplier = 32 * zoomSizeMultiplier;
        }

        map.graphics.setZoom(float(zoomSizeMultiplier));
        auto imageWidth = pixelSizeMultiplier*tileWidth;
        auto imageHeight = pixelSizeMultiplier*tileHeight;
        if ( imageWidth > 16384 || imageHeight > 16384 )
        {
            logger.error() << "Max image dimensions exceeded, failed to generate image for map \"" << map.getFilePath() << "\"\n";
            return;
        }

        GLint savedViewport[4] {};
        glGetIntegerv(GL_VIEWPORT, savedViewport);

        GLuint framebuffer = 0;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        GLuint renderbuffer = 0;
        glGenRenderbuffers(1, &renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, imageWidth, imageHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, renderbuffer);
        glDrawBuffer(GL_COLOR_ATTACHMENT1);

        glViewport(0, 0, imageWidth, imageHeight);
        map.graphics.windowBoundsChanged({.left = 0, .top = 0, .right = imageWidth, .bottom = imageHeight});

        map.graphics.setVerticallyFlipped(true);
        renderMap(map, options);

        glFlush();

        auto start = std::chrono::high_resolution_clock::now();
        glReadBuffer(GL_COLOR_ATTACHMENT1);
        EncodedWebP encodedMapImage {};
        if ( encodeOpenGlViewportImage(0, 0, imageWidth, imageHeight, encodedMapImage) )
            use(encodedMapImage);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        window.pollEvents();
        window.swapBuffers();

        map.graphics.setVerticallyFlipped(false);
        map.graphics.windowBoundsChanged({ savedViewport[0], savedViewport[1], savedViewport[2], savedViewport[3] });
        glViewport(savedViewport[0], savedViewport[1], savedViewport[2], savedViewport[3]);
    }

    struct SaveWebpResult
    {
        int loadSkinAndTilesetTimeMs = 0;
        int encodeTimeMs = 0; // Time for WebPEncodeLosslessRGB https://developers.google.com/speed/webp/docs/api , usually dwarves other times
        int outFileTimeMs = 0; // Time to output to disk
        std::string outputFilePath;
    };

    std::optional<SaveWebpResult> saveMapImageAsWebP(ScMap & map, const Options & options, const std::string & outputFilePath)
    {
        SaveWebpResult result {};
        auto preResourceLoading = std::chrono::high_resolution_clock::now();
        if ( loadSkinAndTileSet(this->renderSkin, map) )
        {
            auto postResourceLoading = std::chrono::high_resolution_clock::now();
            result.loadSkinAndTilesetTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(postResourceLoading-preResourceLoading).count();
        }

        if ( outputFilePath.empty() )
        {
            std::string genFilePath = getDefaultFolder();
            switch ( renderSkin )
            {
                case RenderSkin::ClassicGL: genFilePath += "Classic"; break;
                case RenderSkin::ScrSD: genFilePath += "SD"; break;
                case RenderSkin::ScrHD2: genFilePath += "HD2"; break;
                case RenderSkin::ScrHD: genFilePath += "HD"; break;
                case RenderSkin::CarbotHD2: genFilePath += "CarbotHD2"; break;
                case RenderSkin::CarbotHD: genFilePath += "CarbotHD"; break;
                case RenderSkin::Unknown: case RenderSkin::Total:
                    throw std::logic_error("saveMapImageAsWebP called without properly initializing the renderer skin!");
            }

            auto mapFileName = map.getFileName();
            if ( !mapFileName.empty() )
            {
                genFilePath += " - ";
                if ( genFilePath.size() + mapFileName.size() > 260 )
                    genFilePath += mapFileName.substr(0, 260-mapFileName.size()-5);
                else
                    genFilePath += mapFileName;
            }
            genFilePath += ".webp";
            std::swap(result.outputFilePath, genFilePath);
        }
        else
            result.outputFilePath = outputFilePath;

        bool success = false;
        encodeMapImageAsWebP(map, options, [&](EncodedWebP & encodedWebP) {

            result.encodeTimeMs = encodedWebP.encodeTimeMs;

            auto startFileIo = std::chrono::high_resolution_clock::now();
            auto outputPath = std::filesystem::path(result.outputFilePath);
            std::ofstream outFile(outputPath, std::ios_base::binary|std::ios_base::out);
            outFile.write(reinterpret_cast<const char*>(&encodedWebP.data[0]), std::streamsize(encodedWebP.size));
            encodedWebP = {};
            success = outFile.good();
            outFile.close();
            auto finishFileIo = std::chrono::high_resolution_clock::now();

            result.outFileTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(finishFileIo-startFileIo).count();

            if ( success && outputFilePath.empty() ) // Auto-gen'd, so log the filePath
                logger.info() << "Saved map image to: \"" << result.outputFilePath << "\"\n";
            else if ( !success )
                logger.error() << "Failed to write map image to file: \"" << result.outputFilePath << "\"\n";
        });
        return success ? std::optional<SaveWebpResult>(std::move(result)) : std::nullopt;
    }

    void getMapImageAsWebP(ScMap & map, const Options & options, EncodedWebP & encodedWebP)
    {
        encodeMapImageAsWebP(map, options, [&encodedWebP](EncodedWebP & data) {
            encodedWebP.swap(data);
        });
    }

    void displayInGui(ScMap & map, const Options & options, bool autoAnimate) // Note that this method is blocking, close the window or press escape to continue
    {
        loadSkinAndTileSet(this->renderSkin, map);
        window.show();

        displayedMap = &map;
        bool hasCrgb = map->hasSection(Chk::SectionName::CRGB);

        window.pollEvents();
        processInput();
        showGui = true;
        auto last = std::chrono::system_clock::now();
        while ( showGui && !window.shouldClose() )
        {
            auto now = std::chrono::system_clock::now();
            processInput();

            renderMap(map, options, now);

            glFlush();
            window.pollEvents();
            window.swapBuffers();

            if ( autoAnimate && map.gameClock.tick() )
            {
                map.animations.animate(map.gameClock.currentTick());
                map.graphics.updateGraphics(std::chrono::duration_cast<std::chrono::milliseconds>(now-last).count());
                last = now;
            }
        }
        showGui = false;
    }

    // Renders the map with the current settings, see GuiMapGraphics::render
    void renderMap(ScMap & map, const Options & options, std::chrono::system_clock::time_point frameStart = std::chrono::system_clock::now())
    {
        clearWindow();
        map.graphics.setFrameStart(frameStart);

        if ( options.drawStars && map->tileset == Sc::Terrain::Tileset::SpacePlatform )
        {
            if ( renderSkin == RenderSkin::ClassicGL )
                map.graphics.drawClassicStars();
            else
                map.graphics.drawStars(0xFFFFFFFF);
        }

        if ( options.drawTerrain )
            map.graphics.drawTerrain();

        if ( options.drawFogPlayer )
            map.graphics.drawFog(options.drawFogPlayer.value());

        if ( options.drawActors )
        {
            map.graphics.prepareImageRendering();
            map.graphics.drawActors(map->hasSection(Chk::SectionName::CRGB));
        }

        if ( options.drawLocations )
            map.graphics.drawLocations();

        if ( options.displayFps )
            map.graphics.drawFps();
    }
};

struct GfxUtil
{
    std::unique_ptr<Sc::Data> scData = nullptr;
    std::string starCraftDirectory {};

    void loadScData(const std::string & expectedScPath = ::getDefaultScPath())
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

    std::unique_ptr<Renderer> createRenderer(RenderSkin skin, ScMap & map)
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
    std::unique_ptr<Renderer> createRenderer(RenderSkin skin)
    {
        auto dummyMap = createMap(Sc::Terrain::Tileset::Badlands, 64, 64);
        return createRenderer(skin, *dummyMap);
    }

    // Load map selected from browser
    std::unique_ptr<ScMap> loadMap()
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
    std::unique_ptr<ScMap> loadMap(const std::string & mapFilePath)
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
    std::unique_ptr<ScMap> createMap(Sc::Terrain::Tileset tileset, u16 width = 64, u16 height = 64, size_t terrainTypeIndex = std::numeric_limits<size_t>::max(), SaveType saveType = SaveType::HybridScm)
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

    std::unique_ptr<ScMap> createBlankMap()
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
    std::unique_ptr<ScMap> createUninitializedMap()
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
};

struct MapTimings
{
    int totalTimeMs = 0;
    int mapLoadTimeMs = 0;
    int loadSkinAndTilesetTimeMs = 0;
    int animRealTimeMs = 0;
    int animGameTimeMs = 0;
    int animTicks = 0;
    int encodeTimeMs = 0; // Time for WebPEncodeLosslessRGB https://developers.google.com/speed/webp/docs/api , usually dwarves other times
    int outFileTimeMs = 0; // Time to write to disk

    MapTimings(std::chrono::high_resolution_clock::time_point mapStartTime,
        std::chrono::high_resolution_clock::time_point mapLoadTime,
        ScMap::Simulation animSimTimes,
        std::optional<Renderer::SaveWebpResult> saveWebpResult,
        std::chrono::high_resolution_clock::time_point mapFinishTime) :
            totalTimeMs(std::chrono::duration_cast<std::chrono::milliseconds>(mapFinishTime-mapStartTime).count()),
            mapLoadTimeMs(std::chrono::duration_cast<std::chrono::milliseconds>(mapLoadTime-mapStartTime).count()),
            loadSkinAndTilesetTimeMs(saveWebpResult ? saveWebpResult->loadSkinAndTilesetTimeMs : -1),
            animRealTimeMs(animSimTimes.realTimeSpentMs),
            animGameTimeMs(animSimTimes.gameTimeSimulatedMs),
            animTicks(animSimTimes.ticks),
            encodeTimeMs(saveWebpResult ? saveWebpResult->encodeTimeMs : -1),
            outFileTimeMs(saveWebpResult ? saveWebpResult->outFileTimeMs : -1)
    {}

    inline friend std::ostream & operator<<(std::ostream & os, const MapTimings & time)
    {
        os << "Load skin+tileset: " << time.loadSkinAndTilesetTimeMs << "ms\n"
            << "Map load: " << time.mapLoadTimeMs << "ms\n"
            << "Animation: " << time.animRealTimeMs << "ms (simulated " << time.animTicks << " ticks = " << time.animGameTimeMs << "ms of game time)\n"
            << "WebPEncodeLosslessRGB: " << time.encodeTimeMs << "ms\n"
            << "File output: " << time.outFileTimeMs << "ms\n"
            << "Total: " << time.totalTimeMs << "ms\n";

        return os;
    }
};

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
    //auto map = gfxUtil.loadMap("C:/misc/SimpleLoc.scx"); // Use an absolute file path
    auto map = exampleLoadFromMemory(gfxUtil, tileset); // Use something you already have in memory
    auto mapLoadTime = std::chrono::high_resolution_clock::now();

    auto animTime = map->simulateAnim(52); // Simulate n anim ticks occuring, need at least 52 to extend all the tanks

    //renderer.displayInGui(*map, options, true);
    auto imageTimes = renderer.saveMapImageAsWebP(*map, options, outFilePath);
    auto mapFinishTime = std::chrono::high_resolution_clock::now();
    logger << MapTimings(mapStartTime, mapLoadTime, animTime, imageTimes, mapFinishTime) << std::endl;
}

int main()
{
    auto startInitialLoad = std::chrono::high_resolution_clock::now();
    GfxUtil gfxUtil {};
    gfxUtil.loadScData(); // Could provide a path string to this method
    auto renderer = gfxUtil.createRenderer(RenderSkin::ClassicGL);
    auto endInitialLoad = std::chrono::high_resolution_clock::now();

    logger.info() << "Initial load completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(endInitialLoad-startInitialLoad).count() << "ms" << std::endl;

    testRender(gfxUtil, *renderer, Sc::Terrain::Tileset::SpacePlatform, getDefaultFolder() + "space.webp");
    testRender(gfxUtil, *renderer, Sc::Terrain::Tileset::Jungle, getDefaultFolder() + "jungle.webp");
    testRender(gfxUtil, *renderer, Sc::Terrain::Tileset::SpacePlatform, getDefaultFolder() + "space2.webp");

    return 0;
}
