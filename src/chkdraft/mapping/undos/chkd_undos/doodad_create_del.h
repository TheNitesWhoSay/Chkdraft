#ifndef DOODADCREATEDEL_H
#define DOODADCREATEDEL_H
#include "mapping/undos/reversibles.h"
#include "undo_types.h"

class DoodadCreateDel : public ReversibleAction
{
    public:
        virtual ~DoodadCreateDel();
        static std::shared_ptr<DoodadCreateDel> Make();
        static std::shared_ptr<DoodadCreateDel> Make(u16 doodadIndex, Sc::Terrain::Doodad::Type doodadType, int xStart, int yStart, void *guiMap, bool dd2Only = false); // Undo Creation
        static std::shared_ptr<DoodadCreateDel> Make(u16 doodadIndex, const Chk::Doodad & doodad, bool dd2Only = false); // Undo Deletion
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    protected:
        DoodadCreateDel(u16 doodadIndex, const Chk::Doodad & doodad, bool dd2Only = false);
        DoodadCreateDel(u16 doodadIndex, Sc::Terrain::Doodad::Type type, int xStart, int yStart, void *guiMap, bool dd2Only = false);

    private:
        bool dd2Only = false;
        u16 doodadIndex;
        std::unique_ptr<Chk::Doodad> doodad; // If creation, is a nullptr
        u16 tileIndex[16][16] {};
        // Note: sprite properties are auto-linked using the doodad type and coordinates
};

#endif