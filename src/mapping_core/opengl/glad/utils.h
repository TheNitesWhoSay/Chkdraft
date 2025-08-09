#pragma once
#include <glad/glad.h>
#include <stdexcept>

namespace glad
{
    void loadGL() // If using GLFW this requires a current context to have been set
    {
        int version = gladLoadGL();
        if ( version == 0 )
            throw std::runtime_error("Failed to initialize OpenGL context");
    }
}
