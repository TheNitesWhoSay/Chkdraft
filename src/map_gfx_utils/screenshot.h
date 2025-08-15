#ifndef SCREENSHOT_H
#define SCREENSHOT_H
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

struct EncodedWebP {
    int encodeTimeMs = 0;
    uint8_t* data = nullptr; // Allocated by the WebP library
    std::size_t size = 0;
    EncodedWebP() = default;
    [[nodiscard]] inline operator bool() noexcept { return size != 0; }
    ~EncodedWebP(); // Frees "data" if non-null
    void swap(EncodedWebP & other) {
        std::swap(data, other.data);
        std::swap(size, other.size);
    }
};

bool encodeOpenGlViewportImage(int x, int y, int width, int height, EncodedWebP & encodedWebP);

#endif