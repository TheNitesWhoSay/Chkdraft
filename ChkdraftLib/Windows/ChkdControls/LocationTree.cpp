#include "LocationTree.h"
#include "../../Chkdraft.h"

LocationTree::~LocationTree()
{

}

bool LocationTree::AddThis(HWND hTree, HTREEITEM hParent)
{
    hLocationRoot = hParent;
    return TreeViewControl::SetHandle(hTree);
}

void LocationTree::InsertLocationItem(const std::string & text, u32 index)
{
    if ( index <= TreeDataPortion )
        InsertTreeItem(hLocationRoot, text, index|TreeTypeLocation);
}

void LocationTree::RebuildLocationTree()
{
    EmptySubTree(hLocationRoot);

    for ( size_t i = 1; i <= CM->layers.numLocations(); i++ )
    {
        // In general a location must have a string or non-zero coordinates or a specified elevation
        Chk::LocationPtr loc = CM->layers.getLocation(i);
        if ( (i != Chk::LocationId::Anywhere || !CM->LockAnywhere()) && loc != nullptr &&
            (loc->stringId != 0 || loc->left != 0 || loc->right != 0 || loc->top != 0 || loc->bottom != 0 || loc->elevationFlags != 0) )
        {
            std::shared_ptr<ChkdString> locName = CM->strings.getLocationName<ChkdString>(i);
            if ( locName != nullptr )
                InsertLocationItem(*locName, (u32)i);
            else
                InsertLocationItem("Location " + std::to_string(i), (u32)i);
        }
    }

    if ( CM->getLayer() == Layer::Locations )
        ExpandItem(hLocationRoot);

    RedrawThis();
}
