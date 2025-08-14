#include "screenshot.h"
#include <glad/glad.h>
#include <vector>
#include <webp/encode.h>
#include <filesystem>
#include <fstream>
#include <iostream>

EncodedWebP::~EncodedWebP()
{
    if ( data != nullptr )
        WebPFree(data);
}

bool encodeOpenGlViewportImage(int x, int y, int width, int height, EncodedWebP & encodedWebP)
{
    if ( encodedWebP.data != nullptr || encodedWebP.size != 0 )
        throw std::logic_error("Expected a blank encodedWebP as input!");

    std::vector<GLubyte> pixelData(width*height*3, GLbyte(0));
    glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, &pixelData[0]);

    auto clipWidth = width == 16384 ? 16383 : width; // 1-off from maximum, clip the last pixel-column
    auto clipHeight = height == 16384 ? 16383 : height; // 1-off from maximum, clip the last pixel-row
    auto start = std::chrono::high_resolution_clock::now();
    encodedWebP.size = WebPEncodeLosslessRGB(&pixelData[0], clipWidth, clipHeight, 3*width, &encodedWebP.data); // 22658
    auto end = std::chrono::high_resolution_clock::now();
    if ( encodedWebP.size == 0 )
        return false;
    else
        encodedWebP.encodeTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();

    return true;
}
