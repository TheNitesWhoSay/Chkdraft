#include "doodad_tree.h"
#include "chkdraft/chkdraft.h"

DoodadTree::~DoodadTree()
{

}

bool DoodadTree::AddThis(HWND hTree, HTREEITEM hParent)
{
    hDoodadRoot = hParent;
    return TreeViewControl::SetHandle(hTree);
}

void DoodadTree::UpdateDoodadTree()
{
    EmptySubTree(hDoodadRoot);
    if ( CM != nullptr )
    {
        auto tilesetIndex = CM->getTileset() % Sc::Terrain::NumTilesets;
        auto tileset = chkd.scData.terrain.get(Sc::Terrain::Tileset(tilesetIndex));
        for ( const auto & doodadGroup : tileset.doodadGroups )
        {
            auto groupRoot = InsertTreeItem(hDoodadRoot, doodadGroup.name, TreeTypeCategory | (LPARAM)Layer::Doodads);
            size_t doodadIndex = 0;
            for ( const auto & doodadStartTileGroup : doodadGroup.doodadStartTileGroup )
                InsertTreeItem(groupRoot, doodadGroup.name + " #" + std::to_string(++doodadIndex), doodadStartTileGroup|TreeTypeDoodad);
        }
    }
}
