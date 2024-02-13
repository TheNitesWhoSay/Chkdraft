#include "MainTree.h"
#include "../../Chkdraft.h"

MainTree::~MainTree()
{

}

void MainTree::BuildMainTree()
{
    HWND hWnd = getHandle();

    isomTree.AddThis(hWnd, InsertTreeItem(NULL, "Terrain", (LPARAM)Layer::Terrain));
    doodadTree.AddThis(hWnd, InsertTreeItem(NULL, "Doodads", (LPARAM)Layer::Doodads));
    locTree.AddThis(hWnd, InsertTreeItem(NULL, "Locations", (LPARAM)Layer::Locations));
    unitTree.AddThis(hWnd, InsertTreeItem(NULL, "Units", (LPARAM)Layer::Units));
    spriteTree.AddThis(hWnd, InsertTreeItem(NULL, "Sprites", (LPARAM)Layer::Sprites));
}
