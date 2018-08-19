#ifndef LOCATIONMOVE_H
#define LOCATIONMOVE_H
#include "../Reversibles.h"
#include "UndoTypes.h"

class LocationMove : public ReversibleAction
{
    public:
        virtual ~LocationMove();
        static std::shared_ptr<LocationMove> Make(u16 locationIndex, s32 xChange, s32 yChange);
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    protected:
        LocationMove(u16 locationIndex, s32 xChange, s32 yChange);

    private:
        u16 locationIndex;
        s32 xChange;
        s32 yChange;
};

#endif