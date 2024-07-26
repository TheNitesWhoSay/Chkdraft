#include "SpriteIndexMove.h"
#include "../../../Windows/MainWindows/GuiMap.h"

SpriteIndexMove::~SpriteIndexMove()
{

}

std::shared_ptr<SpriteIndexMove> SpriteIndexMove::Make(u16 oldIndex, u16 newIndex)
{
    return std::shared_ptr<SpriteIndexMove>(new SpriteIndexMove(oldIndex, newIndex));
}

void SpriteIndexMove::Reverse(void *guiMap)
{
    Chk::Sprite preserve = ((GuiMap*)guiMap)->getSprite(newIndex);
    ((GuiMap*)guiMap)->deleteSprite(newIndex);
    ((GuiMap*)guiMap)->insertSprite(oldIndex, preserve);
    ((GuiMap*)guiMap)->GetSelections().sendSpriteMove(oldIndex, newIndex);
    std::swap(oldIndex, newIndex);
}

int32_t SpriteIndexMove::GetType()
{
    return UndoTypes::SpriteChange;
}

SpriteIndexMove::SpriteIndexMove(u16 oldIndex, u16 newIndex)
    : oldIndex(oldIndex), newIndex(newIndex)
{

}

SpriteIndexMoveBoundary::~SpriteIndexMoveBoundary()
{

}

std::shared_ptr<SpriteIndexMoveBoundary> SpriteIndexMoveBoundary::Make()
{
    return std::shared_ptr<SpriteIndexMoveBoundary>(new SpriteIndexMoveBoundary());
}

void SpriteIndexMoveBoundary::Reverse(void *guiMap)
{
    ((GuiMap*)guiMap)->GetSelections().finishSpriteMove();
}

int32_t SpriteIndexMoveBoundary::GetType()
{
    return UndoTypes::SpriteChange;
}

SpriteIndexMoveBoundary::SpriteIndexMoveBoundary()
{

}
