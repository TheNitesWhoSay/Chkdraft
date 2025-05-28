#include "sprite_change.h"
#include <chkdraft/chkdraft.h>
#include <ui/main_windows/gui_map.h>

SpriteChange::~SpriteChange()
{

}

std::shared_ptr<SpriteChange> SpriteChange::Make(size_t spriteIndex, const Chk::Sprite & replacedSprite)
{
    return std::shared_ptr<SpriteChange>(new SpriteChange(spriteIndex, replacedSprite));
}

void SpriteChange::Reverse(void *guiMap)
{
    /*GuiMap & map = *((GuiMap*)guiMap);
    if ( spriteIndex < map.numSprites() )
    {
        auto & sprite = ((GuiMap*)guiMap)->getSprite(spriteIndex);
        std::swap(sprite, this->replacedSprite);
    }*/
}

int32_t SpriteChange::GetType()
{
    return UndoTypes::SpriteChange;
}

SpriteChange::SpriteChange(size_t spriteIndex, const Chk::Sprite & replacedSprite)
    : spriteIndex(spriteIndex), replacedSprite(replacedSprite)
{

}
