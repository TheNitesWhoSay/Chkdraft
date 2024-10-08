#ifndef CUTCOPYPASTECHANGE_H
#define CUTCOPYPASTECHANGE_H
#include "mapping/undos/reversibles.h"
#include "undo_types.h"

class CutCopyPasteChange : public ReversibleAction
{
    public:
        virtual ~CutCopyPasteChange();
        static std::shared_ptr<CutCopyPasteChange> Make();
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    protected:
        CutCopyPasteChange();
};

#endif