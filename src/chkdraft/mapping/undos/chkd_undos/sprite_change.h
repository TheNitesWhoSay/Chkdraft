#ifndef SPRITECHANGE_H
#define SPRITECHANGE_H
#include "../Reversibles.h"
#include "UndoTypes.h"

class SpriteChange : public ReversibleAction
{
    public:
        virtual ~SpriteChange();
        static std::shared_ptr<SpriteChange> Make(size_t spriteIndex, const Chk::Sprite & replacedSprite);
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    protected:
        SpriteChange(size_t spriteIndex, const Chk::Sprite & replacedSprite);

    private:
        size_t spriteIndex;
        Chk::Sprite replacedSprite;
};



#endif