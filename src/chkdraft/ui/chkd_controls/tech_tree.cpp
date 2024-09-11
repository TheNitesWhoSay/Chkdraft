#include "tech_tree.h"
#include "chkdraft/chkdraft.h"

TechTree::~TechTree()
{

}

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
    const std::vector<u32>
        Techs_Zerg = { 11, 16, 14, 17, 12, 32, 18, 15, 13 },
        Techs_Terran = { 9, 6, 2, 34, 7, 1, 30, 10, 24, 4, 3, 0, 5, 8 },
        Techs_Protoss = { 23, 28, 25, 29, 20, 31, 27, 19, 21, 22 },
        Techs_Other = { 26, 33, 35, 36, 37, 38, 39, 40, 41, 42, 43 };

    const std::vector<std::vector<u32>> treeTechs = { Techs_Zerg, Techs_Terran, Techs_Protoss, Techs_Other };
    const char* sTreeTechs[] = { "Zerg", "Terran", "Protoss", "Other" };

    for ( size_t i=0; i<treeTechs.size(); i++ )
    {
        HTREEITEM LastGroup = InsertTreeItem(NULL, sTreeTechs[i], TreeTypeCategory | (LPARAM)Layer::Units);
        const std::vector<u32> techs = treeTechs[i];
        for ( size_t j=0; j<techs.size(); j++ )
        {
            u32 techId = techs[j];
            InsertTreeItem(LastGroup, techNames.at(techId), TreeTypeTech|techId);
        }
    }
}