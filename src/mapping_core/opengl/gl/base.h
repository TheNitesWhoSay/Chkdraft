#pragma once
#include <glad/glad.h>

#ifdef _DEBUG
// Enables debugging for the OpenGL wrapper library
#define WRAP_GL_DEBUG
#endif

namespace gl
{
    template <typename T>
    struct Point2D
    {
        T x {};
        T y {};
    };

    template <typename T>
    struct Rect2D
    {
        T left {};
        T top {};
        T right {};
        T bottom {};
    };

    template <typename T>
    struct Size2D
    {
        T width {};
        T height {};
    };
}
