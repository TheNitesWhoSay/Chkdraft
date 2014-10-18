#include "MainTree.h"
#include "Chkdraft.h"

void MainTree::BuildMainTree()
{
	HWND hWnd = getHandle();

	InsertTreeItem(NULL, "Terrain", LAYER_TERRAIN);
	locTree.AddThis(hWnd, InsertTreeItem(NULL, "Locations", LAYER_LOCATIONS));
	unitTree.AddThis(hWnd, InsertTreeItem(NULL, "Units", LAYER_UNITS));
	
	InsertTreeItem(NULL, "Sprites", LAYER_SPRITES);
	InsertTreeItem(NULL, "Doodads", LAYER_DOODADS);
}
