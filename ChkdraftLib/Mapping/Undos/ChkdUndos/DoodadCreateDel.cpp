#include "DoodadCreateDel.h"
#include "../../../Chkdraft.h"

DoodadCreateDel::~DoodadCreateDel()
{

}

std::shared_ptr<DoodadCreateDel> DoodadCreateDel::Make(u16 index, Sc::Terrain::Doodad::Type doodadType, int xStart, int yStart, void *guiMap)
{
    return std::shared_ptr<DoodadCreateDel>(new DoodadCreateDel(index, doodadType, xStart, yStart, guiMap));
}

std::shared_ptr<DoodadCreateDel> DoodadCreateDel::Make(u16 index, const Chk::Doodad & doodad)
{
    return std::shared_ptr<DoodadCreateDel>(new DoodadCreateDel(index, doodad));
}

void DoodadCreateDel::Reverse(void *guiMap)
{
    GuiMap & map = *((GuiMap*)guiMap);
    const auto & tileset = chkd.scData.terrain.get(map.getTileset());
    auto doodadType = doodad != nullptr ? doodad->type : map.getDoodad(doodadIndex).type;
    if ( auto doodadGroupIndex = tileset.getDoodadGroupIndex(doodadType) )
    {
        const auto & doodadDat = (Sc::Terrain::DoodadCv5 &)tileset.tileGroups[*doodadGroupIndex];
        if ( doodad == nullptr ) // Do delete
        {
            doodad = std::unique_ptr<Chk::Doodad>(new Chk::Doodad);
            *doodad = map.getDoodad(doodadIndex);

            // Restore any MTXM-tiles stored in this structure
            bool evenWidth = doodadDat.tileWidth%2 == 0;
            bool evenHeight = doodadDat.tileHeight%2 == 0;
            auto xStart = evenWidth ? (doodad->xc+16)/32 - doodadDat.tileWidth/2 : doodad->xc/32 - (doodadDat.tileWidth-1)/2;
            auto yStart = evenHeight ? (doodad->yc+16)/32 - doodadDat.tileHeight/2 : doodad->yc/32 - (doodadDat.tileHeight-1)/2;
            auto mapWidth = map.getTileWidth();

            for ( u16 y=0; y<doodadDat.tileHeight; ++y )
            {
                auto yc = yStart+y;
                u16 tileGroupIndex = *doodadGroupIndex + y;
                const auto & tileGroup = tileset.tileGroups[tileGroupIndex];
                for ( u16 x=0; x<doodadDat.tileWidth; ++x )
                {
                    auto xc = xStart+x;
                    if ( tileGroup.megaTileIndex[x] != 0 )
                        std::swap(this->tileIndex[x][y], map.tiles[yc*mapWidth+xc]);
                }
            }
            
            // Delete any matching sprites
            if ( !map.sprites.empty() )
            {
                for ( int i=int(map.sprites.size())-1; i>=0; --i )
                {
                    const auto & sprite = map.sprites[i];
                    if ( sprite.type == doodadDat.overlayIndex && sprite.xc == doodad->xc && sprite.yc == doodad->yc )
                        map.deleteSprite(i);
                }
            }
            map.deleteDoodad(doodadIndex);
        }
        else // Do create
        {
            // Create the sprite if any
            if ( doodadDat.overlayIndex != 0 )
                map.addSprite(Chk::Sprite{Sc::Sprite::Type(doodadDat.overlayIndex), u16(doodad->xc), u16(doodad->yc), 0, 0, doodadDat.flags});

            // Store current MTXM tile in this structure, replace MTXM tiles with the doodads tiles
            bool evenWidth = doodadDat.tileWidth%2 == 0;
            bool evenHeight = doodadDat.tileHeight%2 == 0;
            auto xStart = evenWidth ? (doodad->xc+16)/32 - doodadDat.tileWidth/2 : doodad->xc/32 - (doodadDat.tileWidth-1)/2;
            auto yStart = evenHeight ? (doodad->yc+16)/32 - doodadDat.tileHeight/2 : doodad->yc/32 - (doodadDat.tileHeight-1)/2;

            for ( u16 y=0; y<doodadDat.tileHeight; ++y )
            {
                auto yc = yStart+y; 
                u16 tileGroupIndex = *doodadGroupIndex + y;
                const auto & tileGroup = tileset.tileGroups[tileGroupIndex];
                for ( u16 x=0; x<doodadDat.tileWidth; ++x )
                {
                    auto xc = xStart+x;
                    if ( tileGroup.megaTileIndex[x] != 0 )
                    {
                        this->tileIndex[x][y] = map.getTile(xc, yc, Chk::StrScope::Game);
                        map.setDoodadTile(xc, yc, 16*tileGroupIndex + x);
                    }
                }
            }

            ((GuiMap*)guiMap)->insertDoodad(doodadIndex, *doodad);
            doodad = nullptr;
        }
    }
}

int32_t DoodadCreateDel::GetType()
{
    return UndoTypes::TerrainChange;
}

DoodadCreateDel::DoodadCreateDel(u16 doodadIndex, const Chk::Doodad & doodad) // Undo deletion
    : doodadIndex(doodadIndex), doodad(nullptr)
{
    this->doodad = std::unique_ptr<Chk::Doodad>(new Chk::Doodad);
    (*(this->doodad)) = doodad;
}

DoodadCreateDel::DoodadCreateDel(u16 doodadIndex, Sc::Terrain::Doodad::Type doodadType, int xStart, int yStart, void *guiMap) // Undo creation
    : doodadIndex(doodadIndex), doodad(nullptr)
{
    GuiMap & map = *((GuiMap*)guiMap);
    
    const auto & tileset = chkd.scData.terrain.get(map.getTileset());
    if ( auto doodadGroupIndex = tileset.getDoodadGroupIndex(doodadType) )
    {
        const auto & doodadDat = (Sc::Terrain::DoodadCv5 &)tileset.tileGroups[*doodadGroupIndex];

        // Store MTXM tiles being replaced in this structure
        for ( u16 y=0; y<doodadDat.tileHeight; ++y )
        {
            auto yc = yStart+y; 
            const auto & tileGroup = tileset.tileGroups[*doodadGroupIndex + y];
            for ( u16 x=0; x<doodadDat.tileWidth; ++x )
            {
                auto xc = xStart+x;
                if ( tileGroup.megaTileIndex[x] != 0 )
                    this->tileIndex[x][y] = map.getTile(xc, yc, Chk::StrScope::Game);
            }
        }
    }
}
