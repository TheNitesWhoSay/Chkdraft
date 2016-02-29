#ifndef LOCATIONCREATEDEL_H
#define LOCATIONCREATEDEL_H
#include "Reversibles.h"
#include "UndoTypes.h"

class LocationCreateDel : public ReversibleAction
{
    public:
        LocationCreateDel(u16 locationIndex, ChkLocation &location, std::string &locationName); // Undo Deletion
        LocationCreateDel(u16 locationIndex); // Undo Creation
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    private:
        u16 locationIndex;
        std::unique_ptr<ChkLocation> location; // If creation, is a nullptr
        RawString locationName; // If creation, is empty
};

#endif
