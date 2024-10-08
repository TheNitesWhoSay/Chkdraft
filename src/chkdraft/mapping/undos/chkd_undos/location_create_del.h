#ifndef LOCATIONCREATEDEL_H
#define LOCATIONCREATEDEL_H
#include "mapping/undos/reversibles.h"
#include "undo_types.h"

class LocationCreateDel : public ReversibleAction
{
    public:
        virtual ~LocationCreateDel();
        static std::shared_ptr<LocationCreateDel> Make(u16 locationId, Chk::Location & location, std::string & locationName); // Undo Deletion
        static std::shared_ptr<LocationCreateDel> Make(u16 locationId); // Undo Creation
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    protected:
        LocationCreateDel(u16 locationId, Chk::Location & location, std::string & locationName); // Undo Deletion
        LocationCreateDel(u16 locationId); // Undo Creation

    private:
        u16 locationId;
        std::unique_ptr<Chk::Location> location; // If creation, is a nullptr
        RawString locationName; // If creation, is empty
};

#endif