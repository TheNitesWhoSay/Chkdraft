#include "DoodadChange.h"
#include "../../../Chkdraft.h"
#include "../../../Windows/MainWindows/GuiMap.h"

DoodadChange::~DoodadChange()
{

}

std::shared_ptr<DoodadChange> DoodadChange::Make(u16 doodadIndex, u8 owner, Chk::Doodad::Enabled enabled)
{
    return std::shared_ptr<DoodadChange>(new DoodadChange(doodadIndex, owner, enabled));
}

void DoodadChange::Reverse(void *guiMap)
{
    GuiMap & map = *((GuiMap*)guiMap);
    if ( doodadIndex < map.numDoodads() )
    {
        auto & doodad = ((GuiMap*)guiMap)->getDoodad(doodadIndex);
        std::swap(doodad.owner, this->owner);
        std::swap(doodad.enabled, this->enabled);
        
        const auto & tileset = chkd.scData.terrain.get(map.getTileset());
        if ( auto doodadGroupIndex = tileset.getDoodadGroupIndex(doodad.type) )
        {
            const auto & doodadDat = (Sc::Terrain::DoodadCv5 &)tileset.tileGroups[*doodadGroupIndex];
            if ( !map.sprites.empty() )
            {
                for ( int i=int(map.sprites.size())-1; i>=0; --i )
                {
                    auto & sprite = map.sprites[i];
                    if ( sprite.type == doodadDat.overlayIndex && sprite.xc == doodad.xc && sprite.yc == doodad.yc )
                        sprite.owner = doodad.owner;
                }
            }
        }
    }
}

int32_t DoodadChange::GetType()
{
    return UndoTypes::TerrainChange;
}

DoodadChange::DoodadChange(u16 doodadIndex, u8 owner, Chk::Doodad::Enabled enabled)
    : doodadIndex(doodadIndex), owner(owner), enabled(enabled)
{

}
