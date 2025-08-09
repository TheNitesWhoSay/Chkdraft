#include "screenshot.h"
#include <glfw/context.h>
#include <glfw/window.h>
#include <glad/glad.h>
#include <glad/utils.h>
#include <gl/fps.h>
#include <gl/utils.h>
#include <cross_cut/logger.h>
#include <mapping_core/mapping_core.h>
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

        gl::Fps fps {};
        while ( !window.shouldClose() )
        {
            fps.update(std::chrono::system_clock::now());
            processInput();
            clearWindow();

            // Drawing code goes here

            gl::unbind();
            window.pollEvents();
            window.swapBuffers();
        }
    }
};

int main()
{
    auto scData = std::make_unique<Sc::Data>();
    if ( scData->load() )
        std::cout << "Badlands has " << scData->terrain.get(Sc::Terrain::Tileset::Badlands).tileGroups.size() << " tile groups\n";
    else
        std::cout << "Failed to load scData!\n";

    App{}.run();
    
    return 0;
}
