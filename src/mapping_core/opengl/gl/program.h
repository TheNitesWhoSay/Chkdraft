#pragma once
#include <glad/glad.h>
#include "uniform.h"
#include "unique_resource.h"
#include "shader.h"
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include <unordered_map>

namespace gl
{
    class Program
    {
        unique_resource<GLuint> programId = 0;
        std::vector<Shader> shaders {};
        std::unordered_map<std::string, GLint> uniformLocationCache {};

        void validateIsCurrentProgram(GLint expectedProgramId)
        {
            GLint currentProgram = 0;
            glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
            if ( currentProgram == 0 )
                throw std::logic_error("Program::setUniform(...) was called prior to setting a program");
            else if ( currentProgram != expectedProgramId )
            {
                throw std::logic_error("Program::setUniform(...) was called for program " + std::to_string(expectedProgramId)
                    + " but program " + std::to_string(currentProgram) + " was currently set!");
            }
        }

    public:

        ~Program()
        {
            if ( programId != 0 )
                glDeleteProgram(*programId);
        }

        bool hasShaders()
        {
            return !shaders.empty();
        }

        void create()
        {
            if ( programId != 0 )
                throw std::logic_error("Program already created!");

            this->programId = glCreateProgram();
            if ( auto error = glGetError(); error != GL_NO_ERROR )
                throw std::runtime_error("Error creating program!");
        }

        void attachShader(Shader && shader)
        {
            glAttachShader(*programId, *shader.shaderId);
            if ( glGetError() != GL_NO_ERROR )
                throw std::runtime_error("Error attaching shader!");
            shaders.push_back(std::move(shader));
        }

        void link()
        {
            glLinkProgram(*programId);
            GLint linkStatus = 0;
            glGetProgramiv(*programId, GL_LINK_STATUS, &linkStatus);
            if ( linkStatus != GL_TRUE || glGetError() != GL_NO_ERROR )
                throw std::runtime_error("Error linking program!");
        }

        std::basic_string<GLchar> getInfoLog()
        {
            GLint logLength = 0;
            glGetProgramiv(*programId, GL_INFO_LOG_LENGTH, &logLength);
            if ( logLength > 0 )
            {
                auto logText = std::basic_string<GLchar>(logLength, static_cast<GLchar>('\0'));
                glGetProgramInfoLog(*programId, logLength, NULL, (GLchar*)logText.data());
                if ( logLength > 0 )
                    return logText;
            }
            return std::basic_string<GLchar>{};
        }

        void use()
        {
            glUseProgram(*programId);
            if ( auto error = glGetError(); error != GL_NO_ERROR )
                throw std::runtime_error("Error using program!");
        }
        
        GLint getUniformLocation(const std::string & name)
        {
            #ifdef _DEBUG
            validateIsCurrentProgram(*programId); // Perform this check every time in debug mode
            #endif
                
            if ( auto found = uniformLocationCache.find(name); found != uniformLocationCache.end() )
                return found->second;

            #ifndef _DEBUG
            validateIsCurrentProgram(*programId); // Perform this check on first access only in release mode
            #endif

            // First time trying to access this uniform, need to find the uniform location
            if ( auto location = glGetUniformLocation(*programId, name.c_str()); location != -1 )
            {
                uniformLocationCache.emplace(name, location);
                return location;
            }

            // Failed to get uniform location, diagnose the issue
            {
                if ( *programId == 0 )
                    throw std::logic_error("Program::setUniform(...) called prior to program initialization!");
                else if ( shaders.empty() )
                    throw std::logic_error("Program::setUniform(...) called without attaching any shaders!");

                GLint linkStatus = 0;
                glGetProgramiv(*programId, GL_LINK_STATUS, &linkStatus);
                if ( linkStatus == GL_FALSE )
                    throw std::logic_error("Program::setUniform(...) called prior to program linking!");

                for ( auto & shader : shaders )
                {
                    if ( shader.shaderId == 0 )
                    {
                        throw std::logic_error("Program::setUniform(...) failed for uniform name \"" + name
                            + "\", this is most likely due to an uninitialized shader");
                    }

                    GLint compileStatus = 0;
                    glGetShaderiv(*shader.shaderId, GL_COMPILE_STATUS, &compileStatus);
                    if ( compileStatus == GL_FALSE )
                    {
                        throw std::logic_error("Program::setUniform(...) failed to uniform name \"" + name
                            + "\", this is most likely due to a shader compilation error in shader "
                            + std::to_string(*shader.shaderId));
                    }
                }

                throw std::invalid_argument("Uniform name: \"" + name
                    + "\" could not be set, the uniform name is most likely invalid or may have been unused in the shader.");
            }
        }

        template <typename Uniform>
        void findUniform(Uniform & uniform)
        {
            #ifdef WRAP_GL_DEBUG
            uniform.initialize(getUniformLocation(uniform.getName()), *programId);
            #else
            uniform.initialize(getUniformLocation(uniform.getName()));
            #endif
        }

        template <typename ... Uniforms>
        void findUniforms(Uniforms & ... uniforms)
        {
            (findUniform(uniforms), ...);
        }
    };
}