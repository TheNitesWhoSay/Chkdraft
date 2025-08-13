#include "screenshot.h"
#include <glad/glad.h>
#include <vector>
#include <webp/encode.h>
#include <filesystem>
#include <fstream>
#include <iostream>

std::optional<std::string> saveOpenGlViewportImage(GLint x, GLint y, GLint width, GLint height, const std::string & outputFilePath)
{
    std::vector<GLubyte> pixelData(width*height*3, GLbyte(0));
    glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, &pixelData[0]);

    struct EncodedWebP {
        uint8_t* data = nullptr;
        std::size_t size = 0;
        [[nodiscard]] inline operator bool() noexcept { return size != 0; }
        ~EncodedWebP() { if ( data != nullptr ) WebPFree(data); }
    };
    EncodedWebP output {};

    auto start = std::chrono::high_resolution_clock::now();
    auto clipWidth = width == 16384 ? 16383 : width; // 1-off from maximum, clip the last pixel-column
    auto clipHeight = height == 16384 ? 16383 : height; // 1-off from maximum, clip the last pixel-row
    output.size = WebPEncodeLosslessRGB(&pixelData[0], clipWidth, clipHeight, 3*width, &output.data); // 22658
    auto end = std::chrono::high_resolution_clock::now();
    if ( output.size == 0 )
    {
        std::cout << "Image creation failed!\n";
        return std::nullopt;
    }
    else
        std::cout << "Created image in " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start) << '\n';

    auto outputPath = std::filesystem::path(outputFilePath);
    std::ofstream outFile(outputPath, std::ios_base::binary|std::ios_base::out);
    outFile.write(reinterpret_cast<const char*>(&output.data[0]), std::streamsize(output.size));
    output = {};
    bool success = outFile.good();
    outFile.close();
    return success ? std::make_optional<std::string>((const char*)outputPath.u8string().c_str()) : std::nullopt;
}
