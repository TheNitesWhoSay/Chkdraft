#ifndef LOCATIONCHANGE_H
#define LOCATIONCHANGE_H
#include "../Reversibles.h"
#include "UndoTypes.h"

class LocationChange : public ReversibleAction
{
    public:
        virtual ~LocationChange();
        static std::shared_ptr<LocationChange> Make(u16 locationIndex, Chk::Location::Field field, u32 data);
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    protected:
        LocationChange(u16 locationIndex, Chk::Location::Field field, u32 data);

    private:
        u16 locationIndex;
        Chk::Location::Field field;
        u32 data;
};

#endif