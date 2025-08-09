#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace glfw
{
    struct Context
    {
        Context()
        {
            if ( glfwInit() == GLFW_FALSE )
                throw std::runtime_error("Failed to initialize glfw!");
        }

        ~Context()
        {
            glfwTerminate();
        }
    };
}