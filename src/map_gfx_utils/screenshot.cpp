#include "screenshot.h"
#include <glad/glad.h>
#include <vector>
#include <webp/encode.h>
#include <filesystem>
#include <fstream>

std::optional<std::string> saveOpenGlViewportImage()
{
    GLint viewport[4] {};
    glGetIntegerv(GL_VIEWPORT, &viewport[0]);
    auto width = viewport[2];
    auto height = viewport[3];
    std::vector<GLubyte> pixelData(width*height*3, GLbyte(0));
    glReadPixels(viewport[0], viewport[1], width, height, GL_RGB, GL_UNSIGNED_BYTE, &pixelData[0]);

    struct EncodedWebP {
        uint8_t* data = nullptr;
        std::size_t size = 0;
        [[nodiscard]] inline operator bool() noexcept { return size != 0; }
        ~EncodedWebP() { if ( data != nullptr ) WebPFree(data); }
    };
    EncodedWebP output {};
    output.size = WebPEncodeLosslessRGB(&pixelData[3*width*(height-1)], width, height, -3*width, &output.data);

#ifdef WIN32
    auto testPath = std::filesystem::path("c:/misc/test.webp");
#else
    auto testPath = std::filesystem::path(std::string(getenv("HOME")) + "/test.webp");
#endif
    std::ofstream outFile(testPath, std::ios_base::binary|std::ios_base::out);
    outFile.write(reinterpret_cast<const char*>(&output.data[0]), std::streamsize(output.size));
    output = {};
    bool success = outFile.good();
    outFile.close();
    return success ? std::make_optional<std::string>((const char*)testPath.u8string().c_str()) : std::nullopt;
}
