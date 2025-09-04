#include "renderer.h"
#include <cross_cut/logger.h>

extern Logger logger;

std::string getDefaultFolder();

void getLatoFontData(std::uint8_t const* & latoFontDataPtr, int & latoFontDataSize);

Renderer::Renderer(const Sc::Data & scData) : scData(scData) {}

void Renderer::initialize()
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

void Renderer::processInput()
{
    if ( window.getKey(GLFW_KEY_ESCAPE) == GLFW_PRESS )
        showGui = false;
}

void Renderer::clearWindow()
{
    gl::clearColor(0.0f, 0.0f, 0.0f, 1.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool Renderer::loadSkinAndTileSet(RenderSkin skin, ScMap & map)
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

std::optional<Renderer::SaveWebpResult> Renderer::saveMapImageAsWebP(ScMap & map, const Options & options, const std::string & outputFilePath)
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
    result.renderTimeMs = encodeMapImageAsWebP(map, options, [&](EncodedWebP & encodedWebP) {

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

int Renderer::getMapImageAsWebP(ScMap & map, const Options & options, EncodedWebP & encodedWebP)
{
    return encodeMapImageAsWebP(map, options, [&encodedWebP](EncodedWebP & data) {
        encodedWebP.swap(data);
    });
}

void Renderer::displayInGui(ScMap & map, const Options & options, bool autoAnimate) // Note that this method is blocking, close the window or press escape to continue
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
void Renderer::renderMap(ScMap & map, const Options & options, std::chrono::system_clock::time_point frameStart)
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
