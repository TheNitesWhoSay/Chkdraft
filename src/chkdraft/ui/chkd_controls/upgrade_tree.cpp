#include "upgrade_tree.h"
#include "chkdraft/chkdraft.h"

UpgradeTree::~UpgradeTree()
{

}

bool UpgradeTree::CreateThis(HWND hParent, int x, int y, int width, int height, bool hasButtons, u32 id)
{
    if ( WinLib::TreeViewControl::CreateThis(hParent, x, y, width, height, hasButtons, id) )
    {
        InsertUpgrades();
        return true;
    }
    else
        return false;
}

void UpgradeTree::InsertUpgrades()
{
    const int Upgrades_Zerg[] = { 28, 53, 25, 52, 31, 30, 27, 32, 29, 26, 24, 3, 12, 4, 10, 11 },
              Upgrades_Terran[] = { 22, 18, 51, 54, 23, 17, 21, 20, 0, 7, 2, 9, 1, 8, 19, 16 },
              Upgrades_Protoss[] = { 41, 47, 49, 43, 39, 37, 42, 40, 44, 34, 14, 5, 13, 15, 6, 36, 35, 38, 33 },
              Upgrades_Other[] = { 45, 46, 48, 50, 55, 56, 57, 58, 59, 60 };

    const int* TreeUpgrades[] = { Upgrades_Zerg, Upgrades_Terran, Upgrades_Protoss, Upgrades_Other };
    const int nTreeUpgrades[] = { sizeof(Upgrades_Zerg)/sizeof(const int), sizeof(Upgrades_Terran)/sizeof(const int),
                                  sizeof(Upgrades_Protoss)/sizeof(const int), sizeof(Upgrades_Other)/sizeof(const int) };
    const char* sTreeUpgrades[] = { "Zerg", "Terran", "Protoss", "Other" };

    for ( int i=0; i<sizeof(TreeUpgrades)/sizeof(const int*); i++ )
    {
        HTREEITEM LastGroup = InsertTreeItem(NULL, sTreeUpgrades[i], TreeTypeCategory|(LPARAM)Layer::Units);
        const int* upgrades = TreeUpgrades[i];
        for ( int j=0; j<nTreeUpgrades[i]; j++ )
        {
            u32 upgradeId = upgrades[j];
            InsertTreeItem(LastGroup, upgradeNames.at(upgradeId), TreeTypeUpgrade|upgradeId);
        }
    }
}
