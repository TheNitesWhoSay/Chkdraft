#ifndef MAPIMAGE_H
#define MAPIMAGE_H
#include <mapping_core/mapping_core.h>

struct MapImage
{
    u16 imageId = 0;
    u8 owner = 0;
    s32 xc = 0;
    s32 yc = 0;
    size_t frame = 0;
    bool flipped = false;
};

#endif