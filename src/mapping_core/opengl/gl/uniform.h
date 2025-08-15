#pragma once
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "base.h"
#include "texture.h"
#include <cstdint>
#include <stdexcept>
#include <string>

namespace gl
{
    namespace uniform
    {
        struct Base
        {
        protected:

            #ifdef WRAP_GL_DEBUG
            GLuint programId = 0;
            void validateIsCurrentProgram()
            {
                GLint currentProgram = 0;
                glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
                if ( currentProgram == 0 )
                    throw std::logic_error("Program::setUniform(...) was called prior to setting a program");
                else if ( currentProgram != this->programId )
                {
                    throw std::logic_error("Program::setUniform(...) was called for program " + std::to_string(this->programId)
                        + " but program " + std::to_string(currentProgram) + " was currently set!");
                }
            }
            #define WRAP_GL_VALIDATE_CURRENT_PROGRAM() validateIsCurrentProgram();
            #else
            #define WRAP_GL_VALIDATE_CURRENT_PROGRAM()
            #endif

            std::string name {};
            GLint uniformLocation = -1;

        public:
            Base() = delete;
            Base(const std::string & name) : name(name) {}

            const std::string & getName() const { return name; }
            GLint getLocation() const { return uniformLocation; }

            void initialize(GLint uniformLocation
                #ifdef WRAP_GL_DEBUG
                , GLuint programId
                #endif
            )
            {
                #ifdef WRAP_GL_DEBUG
                if ( this->uniformLocation != -1 )
                    throw std::logic_error("Uniform already initialized!");

                this->programId = programId;
                #endif
                this->uniformLocation = uniformLocation;
            }
        };

        struct Sampler2D : Base // sample2d - 2D texture sampler
        {
            using Base::Base;

            void setSlot(GLint slotIndex)
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                glUniform1i(uniformLocation, slotIndex);
            }
        };

        struct Int : Base // Singular int
        {
            using Base::Base;

            void setValue(GLint value)
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                glUniform1i(uniformLocation, value);
            }
        };

        struct UInt : Base // Singular unsigned int
        {
            using Base::Base;

            void setValue(GLuint value)
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                glUniform1ui(uniformLocation, value);
            }
        };

        struct Float : Base // Singular float
        {
            using Base::Base;

            void setValue(GLfloat value)
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                glUniform1f(uniformLocation, value);
            }
        };

        struct Mat4 : Base // mat4 - 4x4 float matrix
        {
            using Base::Base;

            void setMat4(GLfloat* mat4)
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, mat4);
            }

            void setMat4(GLfloat (&mat4)[4][4])
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &mat4[0][0]);
            }

            void setMat4(const glm::mat4x4 & mat4)
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(mat4));
            }

            void loadIdentity()
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                GLfloat identity[] {
                    1.f, 0.f, 0.f, 0.f,
                    0.f, 1.f, 0.f, 0.f,
                    0.f, 0.f, 1.f, 0.f,
                    0.f, 0.f, 0.f, 1.f
                };
                setMat4(&identity[0]);
            }
        };

        struct Mat2 : Base // mat2 - 2x2 float matrix
        {
            using Base::Base;

            void setMat2(GLfloat* mat2)
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                glUniformMatrix2fv(uniformLocation, 1, GL_FALSE, mat2);
            }

            void setMat2(GLfloat (&mat2)[2][2])
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                glUniformMatrix2fv(uniformLocation, 1, GL_FALSE, &mat2[0][0]);
            }

            void setMat2(const glm::mat2x2 & mat2)
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                glUniformMatrix2fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(mat2));
            }

            void loadIdentity()
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                GLfloat identity[] {
                    1.f, 0.f,
                    0.f, 1.f
                };
                setMat2(&identity[0]);
            }
        };

        struct Vec4 : Base // vec4 - 4 float vector
        {
            using Base::Base;

            void setVec4(GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                glUniform4f(uniformLocation, v0, v1, v2, v3);
            }
        };

        struct Vec2 : Base // vec2 - 2 float vector
        {
            using Base::Base;

            void setVec2(GLfloat x, GLfloat y)
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                glUniform2f(uniformLocation, x, y);
            }
        };

        struct UVec2 : Base // uVec2 - 2 unsigned integer vector
        {
            using Base::Base;

            void setUVec2(GLuint x, GLuint y)
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                glUniform2ui(uniformLocation, x, y);
            }
        };

        struct Vec2Array : Base // vec2Array - array of 2 float vectors
        {
            using Base::Base;

            template <size_t N>
            void setVec2Array(const gl::Point2D<GLfloat> (&vec2Array)[N])
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                glUniform2fv(uniformLocation, N, &vec2Array[0].x);
            }
        };

        struct FloatV4 : Base // vec4 - 4 float vector, but only the first element is used
        {
            using Base::Base;

            void setValue(GLfloat v0)
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                glUniform4f(uniformLocation, v0, 0.f, 0.f, 0.f);
            }
        };

        struct FloatV4_3 : Base // vec4 - 4 float vector, but only the third element is used
        {
            using Base::Base;

            void setValue(GLfloat v2)
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                glUniform4f(uniformLocation, 0.0f, 0.f, v2, 0.f);
            }
        };

        struct Color : Base // vec4 - 4 float vector
        {
            using Base::Base;

            void setColor(std::uint32_t rgbaColor) // uint32 rgbaColor in format 0xAABBGGRR
            {
                WRAP_GL_VALIDATE_CURRENT_PROGRAM();
                glUniform4f(uniformLocation,
                    (rgbaColor&0xFF) / 256.f, // R
                    ((rgbaColor&0xFF00) >> 8) / 256.f, // G
                    ((rgbaColor&0xFF0000) >> 16) / 256.f, // B
                    ((rgbaColor&0xFF000000) >> 24) / 255.f // A
                );
            }
        };
    }
}

#undef WRAP_GL_VALIDATE_CURRENT_PROGRAM
