#pragma once
#include <glad/gl.h>
#include "unique_resource.h"
#include <stdexcept>
#include <string>

namespace gl
{
    class Texture
    {
        struct Tex {
            GLuint id = 0;
            GLenum type = GL_TEXTURE_2D;
            GLsizei total = 0;
        };
        unique_resource<Tex> tex;

    public:

        struct Descriptor
        {
            GLenum format = GL_RGB;
            bool generateMipmap = true;
            GLint wrapS = GL_REPEAT;
            GLint wrapT = GL_REPEAT;
            GLint minFilter = GL_NEAREST_MIPMAP_LINEAR;
            GLint magFilter = GL_NEAREST_MIPMAP_LINEAR;
        };
        
        Texture(GLenum targetType = GL_TEXTURE_2D) : tex{{.type = targetType}} {}
        Texture(Texture &&) noexcept = default;
        Texture & operator=(Texture &&) noexcept = default;
        operator bool() { return tex->id != 0; }

        ~Texture()
        {
            if ( tex->total > 0 && tex->id != 0 )
                glDeleteTextures(tex->total, &tex->id);
        }

        bool empty()
        {
            return tex->total == 0 && tex->id == 0;
        }

        void swap(Texture & other)
        {
            std::swap(tex, other.tex);
        }

        void genTexture() // Generates a single texture
        {
            if ( tex->id != 0 )
                throw std::runtime_error("Texture already generated!");
            else if ( tex->total != 1 )
                tex->total = 1;

            glGenTextures(tex->total, &(tex->id));
        }

        constexpr GLuint getId() { return tex->id; }

        static std::vector<Texture> genTextures(GLsizei totalTextures, GLenum targetType = GL_TEXTURE_2D) // Generates a collection of totalTextures textures
        {
            auto textureIds = std::vector<GLuint>(totalTextures, 0);
            glGenTextures(totalTextures, &textureIds[0]);
            std::vector<Texture> textures {};
            for ( auto textureId : textureIds )
            {
                auto & texture = textures.emplace_back(targetType);
                texture.tex->id = textureId;
                texture.tex->total = 1;
            }
            return textures;
        }

        static void setActiveSlot(GLenum textureUnit)
        {
            glActiveTexture(textureUnit);
        }

        static void bindDefault(GLenum targetType = GL_TEXTURE_2D)
        {
            glBindTexture(targetType, 0);
        }

        static void bindDefaultToSlot(GLenum textureUnit, GLenum targetType = GL_TEXTURE_2D)
        {
            gl::Texture::setActiveSlot(textureUnit);
            gl::Texture::bindDefault(targetType);
        }

        void bind()
        {
            GL_INVALID_ENUM;
            glBindTexture(tex->type, tex->id);
            if ( auto error = glGetError(); error != GL_NO_ERROR ) // GL_INVALID_FRAMEBUFFER_OPERATION = 1286
                int a = 0;
        }

        void bindToSlot(GLenum textureUnit)
        {
            gl::Texture::setActiveSlot(textureUnit);
            bind();
        }

        template <typename T>
        void setParam(GLenum parameter, T && value)
        {
            using type = std::remove_cvref_t<T>;

            if constexpr ( std::is_same_v<type, GLfloat> )
                glTexParameterf(tex->type, parameter, value);
            else if constexpr ( std::is_same_v<type, GLint> )
                glTexParameteri(tex->type, parameter, value);
            else
                static_assert(std::is_void_v<T>, "Expected GL float or int!");
        }

        void setMipmapLevelRange(GLint lowest, GLint highest)
        {
            setParam(GL_TEXTURE_BASE_LEVEL, lowest);
            setParam(GL_TEXTURE_MAX_LEVEL, highest);
        }

        void setMinMagFilters(GLint minFilter, GLint magFilter)
        {
            setParam(GL_TEXTURE_MIN_FILTER, minFilter);
            setParam(GL_TEXTURE_MAG_FILTER, magFilter);
        }

        void setMinMagFilters(GLint filter)
        {
            setMinMagFilters(filter, filter);
        }

        struct Image2D // See glTexImage2D documentation for details
        {
            const void* data;
            GLsizei width = 0;
            GLsizei height = 0;
            GLint level = 0;
            GLint internalformat = GL_RGBA;
            GLenum format = GL_RGBA;
            GLenum type = GL_UNSIGNED_BYTE;
        };

        void loadImage2D(Image2D image)
        {
            glTexImage2D(tex->type, image.level, image.internalformat, image.width, image.height, 0, image.format, image.type, image.data);
        }

        struct SubImage2D // See glTexSubImage2D documentation for details
        {
            const void* pixels = nullptr;
            GLint xoffset = 0;
            GLint yoffset = 0;
            GLsizei width = 0;
            GLsizei height = 0;
            GLint level = 0;
            GLenum format = GL_RGBA;
            GLenum type = GL_UNSIGNED_BYTE;
        };

        void loadSubImage2D(SubImage2D image)
        {
            glTexSubImage2D(tex->type, image.level, image.xoffset, image.yoffset, image.width, image.height, image.format, image.type, image.pixels);
        }

        struct CompressedImage2D // See glCompressedTexImage2D documentation for details
        {
            const void* data = nullptr;
            GLsizei imageSize = 0;
            GLsizei width = 0;
            GLsizei height = 0;
            GLint level = 0;
            GLenum internalformat = 0;
        };

        void loadCompressedImage2D(CompressedImage2D image)
        {
            glCompressedTexImage2D(tex->type, image.level, image.internalformat, image.width, image.height, 0, image.imageSize, image.data);
        }

        struct CompressedSubImage2D // See glCompressedTexSubImage2D documentation for details
        {
            const void* data = nullptr;
            GLsizei imageSize = 0;
            GLint xoffset = 0;
            GLint yoffset = 0;
            GLsizei width = 0;
            GLsizei height = 0;
            GLint level = 0;
            GLenum format = 0;
        };

        void loadCompressedSubImage2D(CompressedSubImage2D image)
        {
            glCompressedTexSubImage2D(tex->type, image.level, image.xoffset, image.yoffset, image.width, image.height, image.format, image.imageSize, image.data);
        }

        // Releases GL_TEXTURE0 through last (inclusive)
        static void releaseSlots(GLenum last)
        {
            for ( GLenum i = GL_TEXTURE0; i <= last; i++ )
                gl::Texture::bindDefaultToSlot(i);
        }
    };
}
