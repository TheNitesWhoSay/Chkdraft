#ifndef LOCATIONCHANGE_H
#define LOCATIONCHANGE_H
#include "mapping/undos/reversibles.h"
#include "undo_types.h"

class LocationChange : public ReversibleAction
{
    public:
        virtual ~LocationChange();
        static std::shared_ptr<LocationChange> Make(u16 locationId, Chk::Location::Field field, u32 data);
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    protected:
        LocationChange(u16 locationId, Chk::Location::Field field, u32 data);

    private:
        u16 locationId;
        Chk::Location::Field field;
        u32 data;
};

#endif