#ifndef FOGCHANGE_H
#define FOGCHANGE_H
#include "mapping/undos/reversibles.h"
#include "undo_types.h"

class FogChange : public ReversibleAction
{
    public:
        virtual ~FogChange();
        static std::shared_ptr<FogChange> Make(u16 tileXc, u16 tileYc, u8 fogValue);
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    protected:
        FogChange(u16 tileXc, u16 tileYc, u8 fogValue);

    private:
        u16 tileXc;
        u16 tileYc;
        u8 fogValue;
};

#endif