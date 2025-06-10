#ifndef MAPIMAGE_H
#define MAPIMAGE_H
#include <mapping_core/mapping_core.h>

struct MapImage
{
    enum class DrawFunction : u8 {
        Normal = 0,
        Shadow = 10,
    };

    u16 imageId = 0;
    u8 owner = 0;
    u8 direction = 0;
    s32 xc = 0;
    s32 yc = 0;
    size_t baseFrame = 0;
    size_t frame = 0;
    bool flipped = false;
    DrawFunction drawFunction = DrawFunction::Normal;

    void playFrame(u8 frame);
    void setDirection(u8 direction);
};

#endif