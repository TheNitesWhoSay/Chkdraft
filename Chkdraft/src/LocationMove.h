#ifndef LOCATIONMOVE_H
#define LOCATIONMOVE_H
#include "Reversibles.h"
#include "UndoTypes.h"

class LocationMove : public ReversibleAction
{
    public:
        LocationMove(u16 locationIndex, s32 xChange, s32 yChange);
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    private:
        u16 locationIndex;
        s32 xChange;
        s32 yChange;
};

#endif
