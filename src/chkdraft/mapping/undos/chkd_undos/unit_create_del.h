#ifndef UNITCREATEDEL_H
#define UNITCREATEDEL_H
#include "mapping/undos/reversibles.h"
#include "undo_types.h"

class UnitCreateDel : public ReversibleAction
{
    public:
        virtual ~UnitCreateDel();
        static std::shared_ptr<UnitCreateDel> Make(u16 index); // Undo Creation
        static std::shared_ptr<UnitCreateDel> Make(u16 index, const Chk::Unit & unit); // Undo Deletion
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();
    
    protected:
        UnitCreateDel(u16 index); // Undo Creation
        UnitCreateDel(u16 index, const Chk::Unit & unit); // Undo Deletion

    private:
        u16 index;
        std::unique_ptr<Chk::Unit> unit; // If creation, is a nullptr
};

#endif