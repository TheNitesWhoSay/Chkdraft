#include "map_image.h"

void MapImage::playFrame(u8 frame)
{
    if ( this->baseFrame != frame )
    {
        this->baseFrame = frame;
        this->frame = this->baseFrame + this->direction;
    }
}

void MapImage::setDirection(u8 direction)
{
    this->direction = (direction+4)/8;
    this->flipped = this->direction > 16;
    if ( flipped )
        this->direction = 32-this->direction;

    frame = baseFrame + this->direction;
}
