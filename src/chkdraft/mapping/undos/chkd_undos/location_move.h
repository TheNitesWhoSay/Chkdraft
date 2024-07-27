#ifndef LOCATIONMOVE_H
#define LOCATIONMOVE_H
#include "mapping/undos/reversibles.h"
#include "undo_types.h"

class LocationMove : public ReversibleAction
{
    public:
        virtual ~LocationMove();
        static std::shared_ptr<LocationMove> Make(u16 locationId, s32 xChange, s32 yChange);
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    protected:
        LocationMove(u16 locationId, s32 xChange, s32 yChange);

    private:
        u16 locationId;
        s32 xChange;
        s32 yChange;
};

#endif