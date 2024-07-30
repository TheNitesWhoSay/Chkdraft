#pragma once
#include "texture.h"
#include <cstdint>

namespace gl
{
    struct Palette
    {
        gl::Texture tex {};
        std::uint32_t colors[256] {};

        constexpr auto & operator[](size_t index) { return colors[index]; }

        inline void update()
        {
            if ( !this->tex ) // Uninitialized
            {
                this->tex.genTexture();
                this->tex.bind();
                this->tex.setMinMagFilters(GL_NEAREST);
                this->tex.loadImage2D({
                    .data = this->colors,
                    .width = 256,
                    .height = 1,
                    .internalformat = GL_RGBA,
                    .format = GL_RGBA
                });
            }
            else
            {
                this->tex.bind();
                this->tex.loadSubImage2D({
                    .pixels = this->colors,
                    .width = 256,
                    .height = 1,
                    .format = GL_RGBA
                });
            }
            gl::Texture::bindDefault();
        }
    };
}
