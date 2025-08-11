#include "screenshot.h"
#include <glfw/context.h>
#include <glfw/window.h>
#include <glad/glad.h>
#include <glad/utils.h>
#include <gl/fps.h>
#include <gl/utils.h>
#include <cross_cut/logger.h>
#include <mapping_core/mapping_core.h>
#include <mapping_core/render/map_animations.h>
#include <mapping_core/render/sc_gl_graphics.h>
#include <iostream>

Logger logger {};

struct App
{
    glfw::Window window {};

    void clearWindow()
    {
        gl::clearColor(0.2f, 0.3f, 0.3f, 1.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void processInput()
    {
        if ( window.getKey(GLFW_KEY_ESCAPE) == GLFW_PRESS )
            window.setWindowShouldClose(true);
        else if ( window.getKey(GLFW_KEY_S) == GLFW_PRESS )
        {
            if ( auto saved = saveOpenGlViewportImage() )
                std::cout << "Saved screenshot to " << *saved << '\n';
        }
    }

    void createMainWindow()
    {
        constexpr int initialWidth = 800;
        constexpr int initialHeight = 600;
        window.setVersionHint(3, 3);
        window.setCoreProfileHint();
        //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Use this to create a hidden window
        window.create(initialWidth, initialHeight, "MapGfxUtils");
        window.enableForwarding(this);

        window.makeContextCurrent();
        glad::loadGL();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        gl::enableDepthTesting();
        gl::setViewport(0, 0, initialWidth, initialHeight);

        window.setFrameBufferSizeCallback<[](GLFWwindow* window, int width, int height) { gl::setViewport(0, 0, width, height); }>();
    }

    void run()
    {
        glfw::Context glfwContext {};
        createMainWindow();
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

        // Begin setup code
        auto scData = std::make_unique<Sc::Data>();
        if ( scData->load() )
            std::cout << "Badlands has " << scData->terrain.get(Sc::Terrain::Tileset::Badlands).tileGroups.size() << " tile groups\n";
        else
            std::cout << "Failed to load scData!\n";

        GameClock gameClock {};

        //Scenario map(Sc::Terrain::Tileset::Installation, 64, 64, Sc::Isom::Brush::Installation::Default, SaveType::HybridScm, &scData->terrain.get(Sc::Terrain::Tileset::Installation));
        //map.addUnit(Chk::Unit{.xc = 64, .yc = 64, .type = Sc::Unit::Type::TerranBattlecruiser});

        MapFile map(MapFile::getDefaultOpenMapBrowser());
        MapAnimations animations(*scData, gameClock, map);
        auto & mapActor = map.view.units.attachedData(0);
        for ( std::size_t i=0; i<map.numUnits(); ++i )
            animations.addUnit(i, map.view.units.attachedData(i));
        for ( std::size_t i=0; i<map.numSprites(); ++i )
            animations.addSprite(i, map.view.sprites.attachedData(i));

        MapGraphics mapGraphics(*scData, map, animations);
        mapGraphics.windowBoundsChanged({
            .left = 0, .top = 0, .right = 800, .bottom = 600
        });
        GraphicsData::LoadSettings loadSettings {
            .visualQuality = VisualQuality::SD,
            .skinId = ::Skin::Id::Classic,
            .tileset = map.getTileset(),
            .forceShowStars = false
        };
        auto fileData = ByteBuffer(1024*1024*120); // 120MB

        bool includesRemastered = false;
        std::shared_ptr<ArchiveCluster> archiveCluster = nullptr;
        if ( loadSettings.skinId == ::Skin::Id::Classic ) // ClassicGL relies only on the general scData loaded earlier
            archiveCluster = std::make_shared<ArchiveCluster>(std::vector<ArchiveFilePtr>{});
        else
        {
            archiveCluster = Sc::DataFile::Browser{}.openScDataFiles(
                includesRemastered,
                Sc::DataFile::getDefaultDataFiles(),
                ::getDefaultScPath(),
                Sc::DataFile::Browser::getDefaultStarCraftBrowser()
            );
        }

        std::unique_ptr<GraphicsData> graphicsData = std::make_unique<GraphicsData>();
        mapGraphics.load(*graphicsData, loadSettings, *archiveCluster, fileData);
        mapGraphics.updateGraphics(1000);
        // End setup code

        gl::Fps fps {};
        while ( !window.shouldClose() )
        {
            fps.update(std::chrono::system_clock::now());
            processInput();
            //clearWindow();
            glClearColor(0.f, 0.f, 0.f, 0.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Begin drawing code
            mapGraphics.drawTerrain();
            mapGraphics.prepareImageRendering();
            mapGraphics.drawActors(map->hasSection(Chk::SectionName::CRGB));
            // End drawing code

            glFlush();
            //gl::unbind();
            window.pollEvents();
            window.swapBuffers();
        }
    }
};

int main()
{
    App{}.run();
    return 0;
}
