#ifndef MTXMCHANGE_H
#define MTXMCHANGE_H
#include "../Reversibles.h"
#include "UndoTypes.h"

class MtxmChange : public ReversibleAction
{
    public:
        virtual ~MtxmChange();
        static std::shared_ptr<MtxmChange> Make(u16 xc, u16 yc, u16 tileValue);
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    protected:
        MtxmChange(u16 xc, u16 yc, u16 tileValue);

    private:
        u16 xc;
        u16 yc;
        u16 tileValue;
};

#endif