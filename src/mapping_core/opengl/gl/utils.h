#pragma once
#include <glad/glad.h>

namespace gl
{
    inline void enableDepthTesting()
    {
        glEnable(GL_DEPTH_TEST);
    }

    inline void setViewport(GLint x, GLint y, GLsizei width, GLsizei height)
    {
        glViewport(x, y, width, height);
    }

    inline void unbind()
    {
        glBindVertexArray(0);
    }

    inline void clearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha, GLbitfield mask)
    {
        glClearColor(red, green, blue, alpha);
        glClear(mask);
    }
}
