#pragma once
// As an abstraction depending on both OpenGL and windows code it should not be organizationally grouped with either
#include "OpenGL/gl/context_semaphore.h"
#include "OpenGL/glad/glad/gl.h"
#include "../../WindowsLib/DeviceContext.h"
#include <memory>
#include <wingdi.h>

// Manages a single win32 OpenGL rendering context (HGLRC) and one or more device contexts
class WglRenderContext : private gl::RenderContext, public gl::ContextSemaphore
{
    std::shared_ptr<WinLib::DeviceContext> deviceContext = nullptr; // Device context is the target (or will become the target on "claim()") for OpenGL ops
    bool deviceContextUsed = false; // If true, deviceContext is the current target for OpenGL ops and "release()" is required before changing deviceContext
    HGLRC renderingContext = NULL; // Handle to the rendering context

protected:
    void claim() override // Claims the OpenGL renderingContext to the current thread/deviceContext
    {
        if ( deviceContextUsed )
            throw std::logic_error("Attempted to claim an OpenGL context which has already been claimed!");
        else if ( deviceContext == nullptr )
            throw std::logic_error("Attempted to claim an OpenGL context without a valid device context being set!");
        else
        {
            wglMakeCurrent(deviceContext->getDcHandle(), renderingContext);
            deviceContextUsed = true;
        }
    }

    void release() override // Releases the OpenGL renderingContext from the current thread/deviceContext
    {
        if ( deviceContextUsed )
        {
            wglMakeCurrent(NULL, NULL);
            deviceContextUsed = false;
        }
    }

public:
    WglRenderContext(std::shared_ptr<WinLib::DeviceContext> deviceContext) // Creates an OpenGL renderingContext with deviceContext as the current rendering target
        : gl::RenderContext{}, gl::ContextSemaphore{(gl::RenderContext*)this}, deviceContext(deviceContext)
    {
        if ( deviceContext == nullptr )
            throw std::invalid_argument("Device context may not be null!");

        this->renderingContext = wglCreateContext(deviceContext->getDcHandle());
        if ( this->renderingContext == NULL )
            throw std::runtime_error("Failed to create a windows-OpenGL rendering context!");
        
        gl::ContextSemaphore::claimContext();

        if ( gladLoaderLoadGL() == 0 )
            throw std::runtime_error("Failed to load OpenGL!");
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        glDisable(GL_DEPTH_TEST);

        glDisable(GL_POLYGON_SMOOTH);
        glDisable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    ~WglRenderContext() override // Destroys the OpenGL renderingContext
    {
        if ( renderingContext != NULL )
        {
            if ( deviceContextUsed )
                wglMakeCurrent(NULL, NULL);

            wglDeleteContext(renderingContext);
        }
    }

    void setDeviceContext(std::shared_ptr<WinLib::DeviceContext> deviceContext)
    {
        if ( deviceContextUsed )
            gl::ContextSemaphore::releaseContext();

        this->deviceContext = deviceContext;

        gl::ContextSemaphore::claimContext();
    }

};
