#ifndef SPRITEINDEXMOVE_H
#define SPRITEINDEXMOVE_H
#include "../Reversibles.h"
#include "UndoTypes.h"

class SpriteIndexMove : public ReversibleAction
{
    public:
        virtual ~SpriteIndexMove();
        static std::shared_ptr<SpriteIndexMove> Make(u16 oldIndex, u16 newIndex);
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    protected:
        SpriteIndexMove(u16 oldIndex, u16 newIndex);

    private:
        u16 oldIndex;
        u16 newIndex;
};

class SpriteIndexMoveBoundary : public ReversibleAction
{
    public:
        virtual ~SpriteIndexMoveBoundary();
        static std::shared_ptr<SpriteIndexMoveBoundary> Make();
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    protected:
        SpriteIndexMoveBoundary();
};

#endif