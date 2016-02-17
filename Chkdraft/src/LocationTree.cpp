#include "LocationTree.h"
#include "Chkdraft.h"

bool LocationTree::AddThis(HWND hTree, HTREEITEM hParent)
{
	hLocationRoot = hParent;
	return TreeViewControl::SetHandle(hTree);
}

void LocationTree::InsertLocationItem(const char* text, u32 index)
{
	if ( index <= TREE_ITEM_DATA )
		InsertTreeItem(hLocationRoot, text, index|TREE_TYPE_LOCATION);
}

void LocationTree::RebuildLocationTree()
{
	EmptySubTree(hLocationRoot);

	buffer& MRGN = CM->MRGN();
	if ( MRGN.exists() )
	{
		ChkLocation* loc;
		ChkdString locName;
		for ( u32 i=0; i<MRGN.size()/CHK_LOCATION_SIZE; i++ )
		{
			// In general a location must have a string or non-zero coordinates or a specified elevation
			if ( ( i != 63 || !CM->LockAnywhere() ) && CM->getLocation(loc, u8(i)) &&
				 ( loc->stringNum != 0 || loc->xc1 != 0 || loc->xc2 != 0 || loc->yc1 != 0 || loc->xc2 != 0 || loc->elevation != 0 ) )
			{
				if ( CM->getLocationName((u16)i, locName) )
					InsertLocationItem(locName.c_str(), i);
				else
					InsertLocationItem(std::string("Location " + std::to_string(i)).c_str(), i);
			}
		}
	}

	if ( CM->getLayer() == LAYER_LOCATIONS )
		ExpandItem(hLocationRoot);

	RedrawThis();
}
