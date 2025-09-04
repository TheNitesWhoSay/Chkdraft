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
            glfwSetErrorCallback([](int errorCode, const char* description) {
                throw std::runtime_error("A glfw error occured: [" + std::to_string(errorCode) + "] " + description);
            });

            if ( glfwInit() == GLFW_FALSE )
                throw std::runtime_error("Failed to initialize glfw!");
        }

        ~Context()
        {
            glfwTerminate();
        }
    };
}