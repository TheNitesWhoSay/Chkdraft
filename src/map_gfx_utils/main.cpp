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

extern const std::uint8_t* latoFontDataPtr;
extern const int latoFontDataSize;

Logger logger {};

enum class RenderSkin
{
    ClassicGL,
    ScrSD,
    ScrHD2,
    ScrHD,
    CarbotHD2,
    CarbotHD,
    Unknown
};

// A StarCraft map including the MapFile (scx/scm/chk), a game clock, animations, and graphics
class ScMap : public MapFile
{
    void init()
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

public:
    GameClock gameClock {};
    MapAnimations animations;
    MapGraphics graphics;

    // Load map at filePath
    ScMap(Sc::Data & scData, const std::string & filePath) :
        MapFile(filePath),
        animations(scData, gameClock, static_cast<const Scenario &>(*this)),
        graphics(scData, static_cast<Scenario &>(*this), animations) { init(); }

    // Load map selected from browser
    ScMap(Sc::Data & scData, FileBrowserPtr<SaveType> fileBrowser /*= MapFile::getDefaultOpenMapBrowser()*/) :
        MapFile(fileBrowser),
        animations(scData, gameClock, static_cast<const Scenario &>(*this)),
        graphics(scData, static_cast<Scenario &>(*this), animations) { init(); }

    // Create a new map
    ScMap(Sc::Data & scData, Sc::Terrain::Tileset tileset, u16 width = 64, u16 height = 64, size_t terrainTypeIndex = 0, SaveType saveType = SaveType::HybridScm, const Sc::Terrain::Tiles* tilesetData = nullptr) :
        MapFile(tileset, width, height, terrainTypeIndex, saveType, tilesetData),
        animations(scData, gameClock, static_cast<const Scenario &>(*this)),
        graphics(scData, static_cast<Scenario &>(*this), animations) { init(); }

    // Creates an uninitialized map that must be manually loaded before use
    ScMap(Sc::Data & scData) :
        MapFile(),
        animations(scData, gameClock, static_cast<const Scenario &>(*this)),
        graphics(scData, static_cast<Scenario &>(*this), animations) { init(); }

    void simulateAnim(std::uint64_t ticks)
    {
        auto start = std::chrono::high_resolution_clock::now();
        auto startTick = gameClock.currentTick();
        for ( auto i=startTick; i<startTick+ticks; ++i )
        {
            gameClock.setTickCount(i);
            animations.animate(i);
        }
        auto finish = std::chrono::high_resolution_clock::now();
        logger.info() << "Simulated " << ticks << " ticks (" << (42*ticks) << "ms game time) in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms of real time.\n";
    }
};

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
    const Sc::Data & scData;
    ScMap* displayedMap = nullptr;
    std::unique_ptr<GraphicsData> graphicsData;
    RenderSkin renderSkin = RenderSkin::Unknown;

    struct Options {
        bool drawTerrain = true;
        bool drawActors = true; // Units & sprites
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
            window.setWindowShouldClose(true);
    }

    void clearWindow()
    {
        gl::clearColor(0.0f, 0.0f, 0.0f, 1.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void loadSkin(RenderSkin skin, ScMap & map)
    {
        if ( this->renderSkin == skin )
            return;

        // Validate the skin and if remastered, turn it into GraphicsData::LoadSettings
        GraphicsData::LoadSettings loadSettings {
            .visualQuality = VisualQuality::SD,
            .skinId = ::Skin::Id::Classic,
            .tileset = Sc::Terrain::Tileset(map.getTileset() % Sc::Terrain::NumTilesets),
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
        else
        {
            archiveCluster = Sc::DataFile::Browser{}.openScDataFiles(
                includesRemastered,
                Sc::DataFile::getDefaultDataFiles(),
                ::getDefaultScPath(), // "/home/nites/StarCraft",
                Sc::DataFile::Browser::getDefaultStarCraftBrowser() // nullptr
            );
        }

        if ( graphicsData == nullptr )
        {
            graphicsData = std::make_unique<GraphicsData>();
            graphicsData->defaultFont = gl::Font::loadStatic(latoFontDataPtr, std::size_t(latoFontDataSize), 0, 12);
        }

        auto loadBuffer = ByteBuffer(1024*1024*120); // 120MB
        map.graphics.load(*graphicsData, loadSettings, *archiveCluster, loadBuffer);
        map.graphics.setFont(graphicsData->defaultFont.get());
        this->renderSkin = skin;
    }

    void saveMapImage(ScMap & map, const Options & options, std::string outputFilePath = "")
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

        if ( outputFilePath.empty() )
        {
            #ifdef WIN32
            outputFilePath = "c:/misc/";
            #else
            outputFilePath = std::string(getenv("HOME")) + "/";
            #endif

            switch ( renderSkin )
            {
                case RenderSkin::ClassicGL: outputFilePath += "Classic "; break;
                case RenderSkin::ScrSD: outputFilePath += "SD "; break;
                case RenderSkin::ScrHD2: outputFilePath += "HD2 "; break;
                case RenderSkin::ScrHD: outputFilePath += "HD "; break;
                case RenderSkin::CarbotHD2: outputFilePath += "CarbotHD2 "; break;
                case RenderSkin::CarbotHD: outputFilePath += "CarbotHD "; break;
            }
            outputFilePath += std::to_string(imageWidth) + "x" + std::to_string(imageHeight);
            outputFilePath += " - ";
            auto mapFileName = map.getFileName();
            if ( outputFilePath.size() + mapFileName.size() > 260 )
                outputFilePath += mapFileName.substr(0, 260-mapFileName.size()-5) + ".webp";
            else
                outputFilePath += mapFileName + ".webp";
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
        saveOpenGlViewportImage(0, 0, imageWidth, imageHeight, outputFilePath);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Saved image in " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start) << '\n';

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        window.pollEvents();
        window.swapBuffers();

        map.graphics.setVerticallyFlipped(false);
        map.graphics.windowBoundsChanged({ savedViewport[0], savedViewport[1], savedViewport[2], savedViewport[3] });
        glViewport(savedViewport[0], savedViewport[1], savedViewport[2], savedViewport[3]);
    }

    void displayInGui(ScMap & map, const Options & options, bool autoAnimate) // Note that this method is blocking, close the window or press escape to continue
    {
        window.show();

        displayedMap = &map;
        bool hasCrgb = map->hasSection(Chk::SectionName::CRGB);
        auto last = std::chrono::system_clock::now();
        while ( !window.shouldClose() )
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
    }

    // Renders the map with the current settings, see GuiMapGraphics::render
    void renderMap(ScMap & map, const Options & options, std::chrono::system_clock::time_point frameStart = std::chrono::system_clock::now())
    {
        clearWindow();
        map.graphics.setFrameStart(frameStart);

        if ( options.drawTerrain )
            map.graphics.drawTerrain();

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

    void loadScData(const std::string & expectedScPath = ::getDefaultScPath())
    {
        auto loadScData = std::make_unique<Sc::Data>();
        if ( loadScData->load(
            std::make_shared<Sc::DataFile::Browser>(),
            Sc::DataFile::getDefaultDataFiles(),
            expectedScPath) )
        {
            std::swap(this->scData, loadScData);
        }
    }

    std::unique_ptr<Renderer> createRenderer(RenderSkin skin, ScMap & map)
    {
        if ( !scData )
        {
            logger.error("scData should be loaded prior to creating a renderer");
            return nullptr;
        }
        auto renderer = std::make_unique<Renderer>(*this->scData);
        renderer->initialize();
        renderer->loadSkin(skin, map);
        return renderer;
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

int main()
{
    GfxUtil gfxUtil {};

    gfxUtil.loadScData();
    //auto map = gfxUtil.loadMap();
    auto map = gfxUtil.loadMap("C:\\Users\\Nites\\Documents\\StarCraft\\Maps\\Download\\LotR March of Sauron L7.scx");
    map->simulateAnim(20);
    auto renderer = gfxUtil.createRenderer(RenderSkin::ClassicGL, *map);

    Renderer::Options options {
        .drawTerrain = true,
        .drawActors = true,
        .drawLocations = false,
        .displayFps = false
    };
    //renderer->displayInGui(*map, options, true);
    renderer->saveMapImage(*map, options);
    return 0;
}
