#ifndef DOODADCHANGE_H
#define DOODADCHANGE_H
#include "mapping/undos/reversibles.h"
#include "undo_types.h"

class DoodadChange : public ReversibleAction
{
    public:
        virtual ~DoodadChange();
        static std::shared_ptr<DoodadChange> Make(u16 doodadIndex, u8 owner, Chk::Doodad::Enabled enabled);
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    protected:
        DoodadChange(u16 doodadIndex, u8 owner, Chk::Doodad::Enabled enabled);

    private:
        u16 doodadIndex;
        u8 owner;
        Chk::Doodad::Enabled enabled;
};

#endif