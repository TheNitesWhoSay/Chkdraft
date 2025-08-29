#include "isom_tree.h"
#include "chkdraft/chkdraft.h"

IsomTree::~IsomTree()
{

}

bool IsomTree::AddThis(HWND hTree, HTREEITEM hParent)
{
    hIsomRoot = hParent;
    return TreeViewControl::SetHandle(hTree);
}

void IsomTree::UpdateIsomTree()
{
    EmptySubTree(hIsomRoot);
    if ( CM != nullptr )
    {
        auto tilesetIndex = CM->getTileset() % Sc::Terrain::NumTilesets;
        auto tileset = chkd.scData->terrain.get(Sc::Terrain::Tileset(tilesetIndex));
        for ( const auto & brush : tileset.brushes )
            InsertTreeItem(hIsomRoot, std::string(brush.name), brush.index|TreeTypeIsom);
    }
}
