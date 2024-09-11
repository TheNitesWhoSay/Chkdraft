#ifndef ISOMCHANGE_H
#define ISOMCHANGE_H
#include "mapping/undos/reversibles.h"
#include "undo_types.h"

class IsomChange : public ReversibleAction
{
    public:
        virtual ~IsomChange();
        static std::shared_ptr<IsomChange> Make(const Chk::IsomRectUndo & isomRectUndo);
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    protected:
        IsomChange(const Chk::IsomRectUndo & isomRectUndo);

    private:
        Chk::IsomRectUndo isomRectUndo;
};

#endif