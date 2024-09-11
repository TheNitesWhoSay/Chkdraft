#ifndef DIMENSIONCHANGE_H
#define DIMENSIONCHANGE_H
#include "mapping/undos/reversibles.h"
#include "undo_types.h"

class DimensionChange : public ReversibleAction
{
    public:
        virtual ~DimensionChange();
        static std::shared_ptr<DimensionChange> Make(void* guiMap);
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();

    protected:
        DimensionChange(void* guiMap);

    private:
        Chk::DIM dimensions {};
        Sc::Terrain::Tileset tileset {Sc::Terrain::Tileset::Badlands};
        std::vector<u16> tiles {};
        std::vector<u16> editorTiles {};
        std::vector<u8> tileFog {};
        std::vector<Chk::IsomRect> isomRects {};
        std::vector<Chk::Doodad> doodads {};
        std::vector<Chk::Sprite> sprites {};
        std::vector<Chk::Unit> units {};
        std::vector<Chk::Location> locations {};
};


#endif