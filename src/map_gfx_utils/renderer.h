#ifndef RENDERER_H
#define RENDERER_H
#include "sc_map.h"
#include "screenshot.h"
#include <glfw/context.h>
#include <glfw/window.h>
#include <glad/glad.h>
#include <glad/utils.h>
#include <gl/utils.h>

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

/* A renderer has one skin+tileset active at a time,
    maps (which have chk data and an anim state) can be passed to a renderer to save screenshots or display the map;
    multiple maps may be used with the same renderer.
   
   Batching Considerations:
    - Renderer options besides the skin can be changed without load time or additional memory use
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

    Renderer(const Sc::Data & scData);

    void initialize();

    void processInput();

    void clearWindow();

    bool loadSkinAndTileSet(RenderSkin skin, ScMap & map);

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
            throw std::logic_error("Map image dimensions exceed maximum for webp format!");
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

    std::optional<SaveWebpResult> saveMapImageAsWebP(ScMap & map, const Options & options, const std::string & outputFilePath);

    void getMapImageAsWebP(ScMap & map, const Options & options, EncodedWebP & encodedWebP);

    void displayInGui(ScMap & map, const Options & options, bool autoAnimate);

    // Renders the map with the current settings, see GuiMapGraphics::render
    void renderMap(ScMap & map, const Options & options, std::chrono::system_clock::time_point frameStart = std::chrono::system_clock::now());
};


#endif