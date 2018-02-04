#ifndef TILECHANGE_H
#define TILECHANGE_H
#include "Reversibles.h"
#include "UndoTypes.h"

class TileChange : public ReversibleAction
{
    public:
        virtual ~TileChange();
        static std::shared_ptr<TileChange> Make(u16 xc, u16 yc, u16 tileValue);
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    protected:
        TileChange(u16 xc, u16 yc, u16 tileValue);

    private:
        u16 xc;
        u16 yc;
        u16 tileValue;
};

#endif