#ifndef UNITCHANGE_H
#define UNITCHANGE_H
#include "Reversibles.h"
#include "UndoTypes.h"

class UnitChange : public ReversibleAction
{
    public:
        UnitChange(u16 unitIndex, ChkUnitField field, u32 data);
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    private:
        u16 unitIndex;
        ChkUnitField field;
        u32 data;
};

#endif