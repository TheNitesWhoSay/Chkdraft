#ifndef UNITCREATEDEL_H
#define UNITCREATEDEL_H
#include "Reversibles.h"
#include "UndoTypes.h"

class UnitCreateDel : public ReversibleAction
{
    public:
        static std::shared_ptr<UnitCreateDel> Make(u16 index); // Undo Creation
        static std::shared_ptr<UnitCreateDel> Make(u16 index, ChkUnit &unit); // Undo Deletion
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();
    
    protected:
        UnitCreateDel(u16 index); // Undo Creation
        UnitCreateDel(u16 index, ChkUnit &unit); // Undo Deletion

    private:
        u16 index;
        std::unique_ptr<ChkUnit> unit; // If creation, is a nullptr
};

#endif