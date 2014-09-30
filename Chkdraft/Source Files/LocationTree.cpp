#include "LocationTree.h"
#include "Chkdraft.h"

bool LocationTree::CreateThis(HWND hParent)
{
	TreeViewControl::SetHandle(hParent);

	locationInsert = InsertParent(hParent, locationInsert, "Locations", LAYER_LOCATIONS);
	hLocationTreeRoot = locationInsert.hParent;

	return true;
}

void LocationTree::InsertLocationItem(const char* text, u32 index)
{
	if ( index <= TREE_ITEM_DATA )
		InsertChild(getHandle(), locationInsert, text, index|TREE_TYPE_LOCATION);
}

void LocationTree::RebuildLocationTree()
{
	GuiMap* map = chkd.maps.curr;
	HTREEITEM hChild = TreeView_GetChild(getHandle(), hLocationTreeRoot);
	while ( hChild != NULL )
	{
		TreeView_DeleteItem(getHandle(), hChild);
		hChild = TreeView_GetChild(getHandle(), hLocationTreeRoot);
	}

	buffer& MRGN = map->MRGN();
	if ( MRGN.exists() )
	{
		ChkLocation* loc;
		std::string locName;
		for ( u32 i=0; i<MRGN.size()/CHK_LOCATION_SIZE; i++ )
		{
			// In general a location must have a string or non-zero coordinates or a specified elevation
			if ( ( i != 63 || !map->LockAnywhere() ) && map->getLocation(loc, u8(i)) &&
				 ( loc->stringNum != 0 || loc->xc1 != 0 || loc->xc2 != 0 || loc->yc1 != 0 || loc->xc2 != 0 || loc->elevation != 0 ) )
			{
				if ( map->getLocationName(locName, u8(i)) )
					InsertLocationItem(locName.c_str(), i);
				else
				{
					char locationName[20];
					sprintf_s(locationName, "Location %u", i);
					InsertLocationItem(locationName, i);
				}
			}
		}
	}

	if ( map->currLayer() == LAYER_LOCATIONS )
		TreeView_Expand(getHandle(), hLocationTreeRoot, TVM_EXPAND);

	RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);
}
