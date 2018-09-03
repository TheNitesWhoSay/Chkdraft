#include "ChkSprite.h"

bool ChkSprite::IsDrawnAsSprite()
{
    return (flags & (u16)SpriteFlags::DrawAsSprite) == (u16)SpriteFlags::DrawAsSprite;
}

bool ChkSprite::IsDisabled()
{
    return (flags & (u16)SpriteFlags::SpriteDisabled) == (u16)SpriteFlags::SpriteDisabled;
}
