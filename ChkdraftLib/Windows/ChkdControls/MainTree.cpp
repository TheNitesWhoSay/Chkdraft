#include "MainTree.h"
#include "../../Chkdraft.h"

MainTree::~MainTree()
{

}

void MainTree::BuildMainTree()
{
    HWND hWnd = getHandle();

    isomTree.AddThis(hWnd, InsertTreeItem(NULL, "Terrain", (LPARAM)Layer::Terrain));
    locTree.AddThis(hWnd, InsertTreeItem(NULL, "Locations", (LPARAM)Layer::Locations));
    unitTree.AddThis(hWnd, InsertTreeItem(NULL, "Units", (LPARAM)Layer::Units));
    
    InsertTreeItem(NULL, "Sprites", (LPARAM)Layer::Sprites);
    InsertTreeItem(NULL, "Doodads", (LPARAM)Layer::Doodads);
}
