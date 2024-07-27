#include "sprite_create_del.h"
#include "ui/main_windows/gui_map.h"

SpriteCreateDel::~SpriteCreateDel()
{

}

std::shared_ptr<SpriteCreateDel> SpriteCreateDel::Make(size_t index)
{
    return std::shared_ptr<SpriteCreateDel>(new SpriteCreateDel(index));
}

std::shared_ptr<SpriteCreateDel> SpriteCreateDel::Make(size_t index, const Chk::Sprite & sprite)
{
    return std::shared_ptr<SpriteCreateDel>(new SpriteCreateDel(index, sprite));
}

void SpriteCreateDel::Reverse(void *guiMap)
{
    if ( sprite == nullptr ) // Do delete
    {
        sprite = std::unique_ptr<Chk::Sprite>(new Chk::Sprite);
        *sprite = ((GuiMap*)guiMap)->getSprite(index);
        ((GuiMap*)guiMap)->deleteSprite(index);
    }
    else // Do create
    {
        ((GuiMap*)guiMap)->insertSprite(index, *sprite);
        sprite = nullptr;
    }
}

int32_t SpriteCreateDel::GetType()
{
    return UndoTypes::SpriteChange;
}

SpriteCreateDel::SpriteCreateDel(size_t index, const Chk::Sprite & sprite) // Undo deletion
    : index(index), sprite(nullptr)
{
    this->sprite = std::unique_ptr<Chk::Sprite>(new Chk::Sprite);
    (*(this->sprite)) = sprite;
}

SpriteCreateDel::SpriteCreateDel(size_t index) // Undo creation
    : index(index), sprite(nullptr)
{

}
