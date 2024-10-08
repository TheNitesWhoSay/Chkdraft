#ifndef UNITCHANGE_H
#define UNITCHANGE_H
#include "mapping/undos/reversibles.h"
#include "undo_types.h"

class UnitChange : public ReversibleAction
{
    public:
        virtual ~UnitChange();
        static std::shared_ptr<UnitChange> Make(u16 unitIndex, Chk::Unit::Field field, u32 data);
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    protected:
        UnitChange(u16 unitIndex, Chk::Unit::Field field, u32 data);

    private:
        u16 unitIndex;
        Chk::Unit::Field field;
        u32 data;
};

#endif