#include "TechTree.h"
#include "Chkdraft.h"

bool TechTree::CreateThis(HWND hParent, int x, int y, int width, int height, bool hasButtons, u32 id)
{
    if ( TreeViewControl::CreateThis(hParent, x, y, width, height, hasButtons, id) )
    {
        InsertTechs();
        return true;
    }
    else
        return false;
}

void TechTree::InsertTechs()
{
    const int Techs_Zerg[] = { 11, 16, 14, 17, 12, 32, 18, 15, 13 },
              Techs_Terran[] = { 9, 6, 2, 34, 7, 1, 30, 10, 24, 4, 3, 0, 5, 8 },
              Techs_Protoss[] = { 23, 28, 25, 29, 20, 31, 27, 19, 21, 22 },
              Techs_Other[] = { 26, 33, 35, 36, 37, 38, 39, 40, 41, 42, 43 };

    const int* TreeTechs[] = { Techs_Zerg, Techs_Terran, Techs_Protoss, Techs_Other };
    const int nTreeTechs[] = { sizeof(Techs_Zerg)/sizeof(const int), sizeof(Techs_Terran)/sizeof(const int),
                               sizeof(Techs_Protoss)/sizeof(const int), sizeof(Techs_Other)/sizeof(const int) };
    const char* sTreeTechs[] = { "Zerg", "Terran", "Protoss", "Other" };

    for ( int i=0; i<sizeof(TreeTechs)/sizeof(const int*); i++ )
    {
        HTREEITEM LastGroup = InsertTreeItem(NULL, sTreeTechs[i], TreeTypeCategory | (LPARAM)Layer::Units);
        const int* techs = TreeTechs[i];
        for ( int j=0; j<nTreeTechs[i]; j++ )
        {
            u32 techId = techs[j];
            InsertTreeItem(LastGroup, techNames.at(techId).c_str(), TreeTypeTech|techId);
        }
    }
}