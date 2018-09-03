#include "UnitTree.h"
#include "../../Chkdraft.h"

UnitTree::~UnitTree()
{

}

bool UnitTree::AddThis(HWND hTree, HTREEITEM hRoot)
{
    if ( TreeViewControl::SetHandle(hTree) )
    {
        InsertAllUnits(hRoot);
        return true;
    }
    else
        return false;
}

bool UnitTree::CreateThis(HWND hParent, int x, int y, int width, int height, bool hasButtons, u32 id)
{
    if ( TreeViewControl::CreateThis(hParent, x, y, width, height, hasButtons, id) )
    {
        InsertAllUnits(NULL);
        return true;
    }
    else
        return false;
}

void UnitTree::UpdateUnitNames(const std::vector<std::string> &displayNames)
{
    unitDisplayNames = displayNames;
}

void UnitTree::UpdateUnitName(int unitId)
{
    if ( unitId < unitDisplayNames.size() )
        TreeViewControl::SetItemText(UnitItems[unitId], unitDisplayNames.at(unitId));
}

void UnitTree::InsertAllUnits(HTREEITEM hParent)
{
    InsertZergItems(hParent);
    InsertTerranItems(hParent);
    InsertProtossItems(hParent);
    InsertNeutralItems(hParent);
    InsertUndefinedItems(hParent);
}

void UnitTree::InsertZergItems(HTREEITEM hParent)
{
    const std::vector<u16>
        Units_Zerg_Misc             = { 59, 36, 35, 97 },
        Units_Zerg_Air              = { 62, 44, 43, 42, 45, 47 },
        Units_Zerg_Buildings        = { 130, 143, 136, 139, 149, 137, 131, 133, 135, 132, 134, 138, 142, 141, 144, 146, 140 },
        Units_Zerg_Ground           = { 50, 40, 46, 41, 38, 103, 39, 37 },
        Units_Zerg_Heroes           = { 54, 53, 104, 51, 56, 55, 49, 48, 52, 57 },
        Units_Zerg_Special          = { 194, 197, 191 },
        Units_Zerg_SpecialBuildings = { 150, 201, 151, 152, 148, 147 },
        Units_Zerg_Zerg             = { 145, 153 };

    const std::vector<std::vector<u16>> treeZerg = {
        Units_Zerg_Misc, Units_Zerg_Air, Units_Zerg_Buildings, Units_Zerg_Ground,
        Units_Zerg_Heroes, Units_Zerg_Special, Units_Zerg_SpecialBuildings, Units_Zerg_Zerg
    };

    const char* sTreeZerg[] = {
        "*", "Air Units", "Buildings",
        "Ground Units", "Heroes", "Special",
        "Special Buildings", "Zerg" };

    HTREEITEM LastRace = InsertTreeItem(hParent, "Zerg", TreeTypeCategory | (LPARAM)Layer::Units);
    for ( int i=0; i<treeZerg.size(); i++ )
    {
        HTREEITEM LastGroup = InsertTreeItem(LastRace, sTreeZerg[i], TreeTypeCategory | (LPARAM)Layer::Units);
        AddUnitItems(LastGroup, treeZerg[i]);
    }
}

void UnitTree::InsertTerranItems(HTREEITEM hParent)
{
    const std::vector<u16>
        Units_Terran_Misc             = { 18, 24, 26, 4, 33, 6, 31, 91, 92, 119 },
        Units_Terran_Addons           = { 107, 115, 117, 120, 108, 118 },
        Units_Terran_Air              = { 12, 11, 9, 58, 8 },
        Units_Terran_Buildings        = { 112, 123, 111, 125, 106, 122, 113, 124, 110, 116, 114, 109 },
        Units_Terran_Ground           = { 32, 1, 3, 0, 34, 7, 30, 5, 2 },
        Units_Terran_Heroes           = { 17, 100, 27, 25, 23, 102, 10, 28, 20, 19, 22, 29, 99, 16, 15, 21 },
        Units_Terran_Special          = { 14, 195, 198, 192, 13 },
        Units_Terran_SpecialBuildings = { 127, 126, 200, 190 },
        Units_Terran_Terran           = { 121 };

    const std::vector<std::vector<u16>> treeTerran = {
        Units_Terran_Misc, Units_Terran_Addons, Units_Terran_Air,
        Units_Terran_Buildings, Units_Terran_Ground, Units_Terran_Heroes,
        Units_Terran_Special, Units_Terran_SpecialBuildings, Units_Terran_Terran
    };

    const char* sTreeTerran[] = {
        "*", "Addons", "Air Units",
        "Buildings", "Ground Units", "Heroes",
        "Special", "Special Buildings", "Terran" };

    HTREEITEM LastRace = InsertTreeItem(hParent, "Terran", TreeTypeCategory | (LPARAM)Layer::Units);
    for ( int i=0; i<treeTerran.size(); i++ )
    {
        HTREEITEM LastGroup = InsertTreeItem(LastRace, sTreeTerran[i], TreeTypeCategory | (LPARAM)Layer::Units);
        AddUnitItems(LastGroup, treeTerran[i]);
    }
}

void UnitTree::InsertProtossItems(HTREEITEM hParent)
{
    const std::vector<u16>
        Units_Protoss_Air              = { 71, 72, 60, 73, 84, 70, 69 },
        Units_Protoss_Buildings        = { 170, 157, 163, 164, 169, 166, 160, 154, 159, 162, 156, 155, 171, 172, 167, 165 },
        Units_Protoss_Ground           = { 68, 63, 61, 66, 67, 64, 83, 85, 65 },
        Units_Protoss_Heroes           = { 87, 88, 86, 74, 78, 77, 82, 80, 98, 79, 76, 81, 75 },
        Units_Protoss_Protoss          = { 158, 161 },
        Units_Protoss_Special          = { 196, 199, 193 },
        Units_Protoss_SpecialBuildings = { 173, 174, 168, 189, 175 };

    const std::vector<std::vector<u16>> treeProtoss = {
        Units_Protoss_Air, Units_Protoss_Buildings, Units_Protoss_Ground,
        Units_Protoss_Heroes, Units_Protoss_Protoss, Units_Protoss_Special,
        Units_Protoss_SpecialBuildings
    };

    const char* sTreeProtoss[] = {
        "Air Units", "Buildings", "Ground Units",
        "Heroes", "Protoss", "Special",
        "Special Buildings" };

    HTREEITEM LastRace = InsertTreeItem(hParent, "Protoss", TreeTypeCategory | (LPARAM)Layer::Units);
    for ( int i=0; i<treeProtoss.size(); i++ )
    {
        HTREEITEM LastGroup = InsertTreeItem(LastRace, sTreeProtoss[i], TreeTypeCategory | (LPARAM)Layer::Units);
        AddUnitItems(LastGroup, treeProtoss[i]);
    }
}

void UnitTree::InsertNeutralItems(HTREEITEM hParent)
{
    const std::vector<u16>
        Units_Neutral_Critters      = { 90, 94, 95, 89, 93, 96 },
        Units_Neutral_Doodads       = { 209, 204, 203, 207, 205, 211, 210, 208, 206, 213, 212 },
        Units_Neutral_Neutral       = { 181, 179, 180, 185, 187, 182, 186 },
        Units_Neutral_Powerups      = { 218, 129, 219, 217, 128, 216 },
        Units_Neutral_Protoss       = { 105 },
        Units_Neutral_Resources     = { 220, 221, 176, 177, 178, 188, 222, 223, 224, 225, 226, 227 },
        Units_Neutral_Special       = { 215, 101 },
        Units_Neutral_StartLocation = { 214 },
        Units_Neutral_Zerg          = { 202 };

    const std::vector<std::vector<u16>> treeNeutral = {
        Units_Neutral_Critters, Units_Neutral_Doodads, Units_Neutral_Neutral,
        Units_Neutral_Powerups, Units_Neutral_Protoss, Units_Neutral_Resources,
        Units_Neutral_Special, Units_Neutral_StartLocation, Units_Neutral_Zerg
    };
    const char* sTreeNeutral[] = {
        "Critters", "Doodads", "Neutral",
        "Powerups", "Protoss", "Resources",
        "Special", "Start Location", "Zerg" };

    HTREEITEM LastRace = InsertTreeItem(hParent, "Neutral", TreeTypeCategory | (LPARAM)Layer::Units);
    for ( int i=0; i<treeNeutral.size(); i++ )
    {
        HTREEITEM LastGroup = InsertTreeItem(LastRace, sTreeNeutral[i], TreeTypeCategory | (LPARAM)Layer::Units);
        AddUnitItems(LastGroup, treeNeutral[i]);
    }
}

void UnitTree::InsertUndefinedItems(HTREEITEM hParent)
{
    const std::vector<u16> Units_Independent = { 183, 184 };

    const std::vector<std::vector<u16>> treeUndefined = { Units_Independent };
    const char* sTreeUndefined[] = { "Independent" };

    HTREEITEM LastRace = InsertTreeItem(hParent, "Undefined", TreeTypeCategory | (LPARAM)Layer::Units);
    for ( int i=0; i<treeUndefined.size(); i++ )
    {
        HTREEITEM LastGroup = InsertTreeItem(LastRace, sTreeUndefined[i], TreeTypeCategory | (LPARAM)Layer::Units);
        AddUnitItems(LastGroup, treeUndefined[i]);
    }
}

void UnitTree::AddUnitItems(HTREEITEM hParent, const std::vector<u16> &unitIds)
{
    size_t maxUnits = std::min(unitIds.size(), unitDisplayNames.size());
    for ( size_t i=0; i<maxUnits; i++ )
        UnitItems[unitIds[i]] = InsertTreeItem(hParent, unitDisplayNames[unitIds[i]], TreeTypeUnit | (DWORD)unitIds[i]);
}
