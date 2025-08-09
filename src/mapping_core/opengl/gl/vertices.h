#pragma once
#include <glad/glad.h>
#include <rarecpp/reflect.h>
#include <array>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace gl
{
    struct VertexAttribute { // See glVertexAttribPointer
        GLint size;
        GLenum type = GL_FLOAT;
        GLboolean normalized = GL_FALSE;
        mutable void* pointer = nullptr; // Ignored/calculated by describe
    };

    enum class UsageHint : GLenum {
        StreamDraw = GL_STREAM_DRAW, // Modified once, used at most a few times; modified by application, used as source for GL drawing
        StreamRead = GL_STREAM_READ, // Modified once, used at most a few times; modified by reading data from GL and returned to application
        StreamCopy = GL_STREAM_COPY, // Modified once, used at most a few times; modified by reading data from GL and used as source for GL drawing
        StaticDraw = GL_STATIC_DRAW, // Modified once, used many times; modified by application, used as source for GL drawing
        StaticRead = GL_STATIC_READ, // Modified once, used many times; modified by reading data from GL and returned to application
        StaticCopy = GL_STATIC_COPY, // Modified once, used many times; modified by reading data from GL and used as source for GL drawing
        DynamicDraw = GL_DYNAMIC_DRAW, // Modified repeatedly, used many times; modified by application, used as source for GL drawing
        DynamicRead = GL_DYNAMIC_READ, // Modified repeatedly, used many times; modified by reading data from GL and returned to application
        DynamicCopy = GL_DYNAMIC_COPY // Modified repeatedly, used many times; modified by reading data from GL and used as source for GL drawing
    };

    /**
    * Vertices manages a collection of points and assists in...
    * - Building the vertex collection
    * - Describing the vertex collection with a VAO ("vertex array object")
    * - Sending the collection to a GPU buffer/VAO ("vertex buffer object")
    * - Drawing the collection (sending from GPU buffer to vertex and eventually fragment shaders)
    */
    template <typename ElementType = GLfloat, std::size_t ArraySize = 0>
    struct VerticesBase 
    {
        std::conditional_t<(ArraySize > 0), std::array<ElementType, ArraySize>, std::vector<ElementType>> vertices {};

    private:
        bool buffered = false;
        GLuint vao = 0;
        GLuint vbo = 0;
        GLsizei elementsPerVertex = 0;

    public:
        constexpr operator bool() { return vbo != 0; }

        ~VerticesBase()
        {
            if ( vbo != 0 )
                glDeleteBuffers(1, &vbo);

            if ( vao != 0 )
                glDeleteVertexArrays(1, &vao);
        }

        void bind()
        {
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
        }

        template <size_t N>
        void initialize(const VertexAttribute (&vertexAttributes)[N])
        {
            if ( vbo != 0 ) // Already initialized
                return;

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            bind();
            auto getTypeSize = [](GLenum type) -> GLsizei {
                switch ( type )
                {
                    case GL_BYTE: return 1;
                    case GL_UNSIGNED_BYTE: return 1;
                    case GL_SHORT: return 2;
                    case GL_UNSIGNED_SHORT: return 2;
                    case GL_INT: return 4;
                    case GL_UNSIGNED_INT: return 4;
                    case GL_HALF_FLOAT: return 2;
                    case GL_FLOAT: return 4;
                    case GL_DOUBLE: return 8;
                    default: throw std::invalid_argument("Vertex attribute type unrecognized!");
                }
            };
            
            this->elementsPerVertex = 0;
            GLsizei offset = 0;
            for ( auto & vertexAttribute : vertexAttributes )
            {
                if ( vertexAttribute.size < 1 || vertexAttribute.size > 4 )
                    throw std::invalid_argument("Size must be between 1 and 4");
                
                this->elementsPerVertex += vertexAttribute.size;
                vertexAttribute.pointer = (void*)(size_t)offset;
                offset += getTypeSize(vertexAttribute.type)*vertexAttribute.size;
            }

            for ( size_t i=0; i<N; ++i )
            {
                auto & vertexAttribute = vertexAttributes[i];
                glVertexAttribPointer(GLuint(i), vertexAttribute.size, vertexAttribute.type, vertexAttribute.normalized, offset, vertexAttribute.pointer);
                glEnableVertexAttribArray(GLuint(i));
            }
        }

        static void unbind()
        {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        void bufferData(UsageHint usageHint)
        {
            glBufferData(GL_ARRAY_BUFFER, std::size(vertices)*sizeof(ElementType), &vertices[0], GLenum(usageHint));
            buffered = true;
        }

        void bufferSubData(UsageHint usageHint)
        {
            if ( !buffered )
                bufferData(usageHint);

            glBufferSubData(GL_ARRAY_BUFFER, 0, std::size(vertices)*sizeof(ElementType), &vertices[0]);
        }

        void drawTriangles()
        {
            glDrawArrays(GL_TRIANGLES, 0, GLsizei(std::size(vertices))/elementsPerVertex);
        }

        void drawLines()
        {
            glDrawArrays(GL_LINES, 0, GLsizei(std::size(vertices))/elementsPerVertex);
        }

        void clear()
        {
            if constexpr ( RareTs::has_clear_v<std::remove_cv_t<decltype(vertices)>> )
                vertices.clear();
        }
    };

    template <typename ElementType = GLfloat>
    using VertexVector = VerticesBase<ElementType, 0>;

    template <std::size_t size = 0, typename ElementType = GLfloat>
    using VertexArray = VerticesBase<ElementType, size>;
}
