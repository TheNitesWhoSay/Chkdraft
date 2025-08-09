#pragma once
#include <glad/glad.h>
#include "unique_resource.h"
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace gl
{
    class Program;

    class Shader
    {
        unique_resource<GLuint> shaderId = 0;

        std::basic_string<GLchar> getInfoLog()
        {
            GLint logLength = 0;
            glGetShaderiv(*shaderId, GL_INFO_LOG_LENGTH, &logLength);
            if ( logLength > 0 )
            {
                auto logText = std::basic_string<GLchar>(logLength, static_cast<GLchar>('\0'));
                glGetShaderInfoLog(*shaderId, logLength, NULL, (GLchar*)logText.data());
                if ( logLength > 0 )
                    return logText;
            }
            return std::basic_string<GLchar>{};
        }

    public:

        enum class Type : GLenum
        {
            fragment = GL_FRAGMENT_SHADER,
            vertex = GL_VERTEX_SHADER
        };

        Shader() = default;
        Shader(Shader &&) noexcept = default;
        Shader & operator=(Shader &&) = default;

        Shader(Shader::Type type, const char* source, size_t sourceLength, bool requireNonEmpty)
        {
            if ( requireNonEmpty && (source == nullptr || source[0] == '\0') )
                throw std::runtime_error("The shader source cannot be empty!");

            GLint length = static_cast<GLint>(sourceLength);
            shaderId = glCreateShader(static_cast<GLenum>(type));
            //logger.info() << std::string_view(source, length);
            glShaderSource(*shaderId, 1, (const GLchar* const*)&source, &length);
            glCompileShader(*shaderId);

            GLint status = 0;
            glGetShaderiv(*shaderId, GL_COMPILE_STATUS, &status);
            if ( status == GL_FALSE )
            {
                std::string infoLog = getInfoLog();
                throw std::runtime_error("Failed to compile shader: \n\n" + infoLog);
            }
        }

        Shader(Shader::Type type, std::string_view source, bool requireNonEmpty = true)
            : Shader(type, source.data(), source.size(), requireNonEmpty) {}

        ~Shader()
        {
            if ( shaderId != 0 )
                glDeleteShader(*shaderId);
        }

        friend class gl::Program; // Program requires access to the underlying shaderId for the attach function and to perform diagnostics
    };
    
    template <Shader::Type ShaderType>
    inline Shader shaderFromFile(const std::string & filePath, bool requireNonEmpty = true)
    {
        std::ifstream shaderFile {};
        shaderFile.open(filePath);
        if ( !shaderFile )
        {
            shaderFile.clear();
            shaderFile.open("../" + filePath); // Try one directory up
            if ( !shaderFile )
                throw std::runtime_error("File not found!");
        }

        std::vector<char> buffer{std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>()};
        if ( !shaderFile )
            throw std::runtime_error("Read error!");

        return Shader(ShaderType, buffer.data(), buffer.size(), requireNonEmpty);
    }

    inline Shader vertexShaderFromFile(const std::string & filePath, bool requireNonEmpty = true)
    {
        return shaderFromFile<Shader::Type::vertex>(filePath, requireNonEmpty);
    }

    inline Shader fragmentShaderFromFile(const std::string & filePath, bool requireNonEmpty = true)
    {
        return shaderFromFile<Shader::Type::fragment>(filePath, requireNonEmpty);
    }

}
