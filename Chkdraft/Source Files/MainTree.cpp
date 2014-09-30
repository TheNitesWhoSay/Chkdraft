#include "MainTree.h"
#include "Chkdraft.h"

void MainTree::BuildMainTree()
{
	HWND hWnd = getHandle();
	TV_INSERTSTRUCT tvinsert = { };

	tvinsert = InsertParent(hWnd, tvinsert, "Terrain", LAYER_TERRAIN);
	locTree.CreateThis(hWnd);
	unitTree.CreateThis(hWnd);
	
	tvinsert = InsertParent(hWnd, tvinsert, "Sprites", LAYER_SPRITES);
	tvinsert = InsertParent(hWnd, tvinsert, "Doodads", LAYER_DOODADS);
}
