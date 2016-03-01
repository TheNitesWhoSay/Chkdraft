#ifndef LOCATIONCHANGE_H
#define LOCATIONCHANGE_H
#include "Reversibles.h"
#include "UndoTypes.h"

class LocationChange : public ReversibleAction
{
    public:
        LocationChange(u16 locationIndex, u8 field, u32 data);
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    private:
        u16 locationIndex;
        u8 field;
        u32 data;
};

#endif