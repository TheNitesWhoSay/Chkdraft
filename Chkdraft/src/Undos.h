#ifndef UNDOS_H
#define UNDOS_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "Reversibles.h"
#include "Selections.h"
#include <map>

class IObserveUndos
{
    public:
        /** Sent whenever a change is made, provided it does
            not meet the criteria for ChangesReversed */
        virtual void ChangesMade() = 0;

        /** Sent if all changes to unique types have been reversed
            since load or since the last call to ResetChangeCount */
        virtual void ChangesReversed() = 0;
};

class Undos
{
    public:

        Undos(IObserveUndos &observer);
        ~Undos();

        void AddUndo(ReversiblePtr action);
        void doUndo(int32_t type, void *obj);
        void doRedo(int32_t type, void *obj);
        void ResetChangeCount(); // Does not trigger notifications

    protected:

        ReversiblePtr popUndo(int32_t type);
        ReversiblePtr popRedo(int32_t type);
        void AdjustChangeCount(int32_t type, int32_t adjustBy);

    private:
        
        IObserveUndos &observer;
        std::list<ReversiblePtr> undos; // front = next undo
        std::list<ReversiblePtr> redos; // front = next redo
        std::map<int32_t, int32_t> changeCounters; // <type, numChanges>, 1 addition/redo = +1 change, 1 undo = -1 change
};

#endif
