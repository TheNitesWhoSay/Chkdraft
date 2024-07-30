#include "location_tree.h"
#include "chkdraft/chkdraft.h"

LocationTree::~LocationTree()
{

}

bool LocationTree::AddThis(HWND hTree, HTREEITEM hParent)
{
    hLocationRoot = hParent;
    return TreeViewControl::SetHandle(hTree);
}

HTREEITEM LocationTree::InsertLocationItem(const std::string & text, u32 index)
{
    if ( index <= TreeDataPortion )
        return InsertTreeItem(hLocationRoot, text, index|TreeTypeLocation);
    else
        return NULL;
}

void LocationTree::RebuildLocationTree(bool updateSelection)
{
    size_t selectedLocation = CM->GetSelections().getSelectedLocation();
    EmptySubTree(hLocationRoot);

    HTREEITEM selected = NULL;
    for ( size_t i = 1; i <= CM->numLocations(); i++ )
    {
        // In general a location must have a string or non-zero coordinates or a specified elevation
        const Chk::Location & loc = CM->getLocation(i);
        if ( (i != Chk::LocationId::Anywhere || !CM->LockAnywhere()) &&
            (loc.stringId != 0 || loc.left != 0 || loc.right != 0 || loc.top != 0 || loc.bottom != 0 || loc.elevationFlags != 0) )
        {
            if ( auto locName = CM->getLocationName<ChkdString>(i) )
            {
                auto inserted = InsertLocationItem(*locName, (u32)i);
                if ( i == selectedLocation )
                    selected = inserted;
            }
            else
            {
                auto inserted = InsertLocationItem("Location " + std::to_string(i), (u32)i);
                if ( i == selectedLocation )
                    selected = inserted;
            }
        }
    }

    if ( CM->getLayer() == Layer::Locations )
        ExpandItem(hLocationRoot);

    if ( updateSelection )
        TreeViewControl::SelectItem(selected != NULL ? selected : hLocationRoot);

    RedrawThis();
}
