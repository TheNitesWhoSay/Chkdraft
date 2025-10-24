#include "unit_tree.h"
#include "chkdraft/chkdraft.h"

UnitTree::~UnitTree()
{

}

bool UnitTree::AddThis(HWND hTree, HTREEITEM hRoot)
{
    if ( TreeViewControl::SetHandle(hTree) )
    {
        hUnitRoot = hRoot;
        return true;
    }
    else
        return false;
}

bool UnitTree::CreateThis(HWND hParent, int x, int y, int width, int height, bool hasButtons, u32 id)
{
    if ( TreeViewControl::CreateThis(hParent, x, y, width, height, hasButtons, id) )
        return true;
    else
        return false;
}

void UnitTree::UpdateUnitTree()
{
    TreeViewControl::EmptySubTree(hUnitRoot);
    InsertAllUnits(hUnitRoot);
}

void UnitTree::UpdateUnitNames(const std::vector<std::string> & displayNames)
{
    unitDisplayNames = displayNames;
}

void UnitTree::UpdateUnitName(int unitId)
{
    if ( (size_t)unitId < unitDisplayNames.size() )
        TreeViewControl::SetItemText(unitItems[unitId], unitDisplayNames.at(unitId));
}

void UnitTree::insertUnits(const Sc::Unit::UnitGroup & unitGroup, HTREEITEM hParent)
{
    HTREEITEM hGroupItem = InsertTreeItem(hParent, unitGroup.groupName, TreeTypeCategory | (LPARAM)Layer::Units);
    for ( auto & subGroup : unitGroup.subGroups )
        insertUnits(subGroup, hGroupItem);

    AddUnitItems(hGroupItem, unitGroup.memberUnits);
}

void UnitTree::insertUnits(const TreeGroup & unitGroup, HTREEITEM hParent)
{
    HTREEITEM hGroupItem = InsertTreeItem(hParent, unitGroup.label, TreeTypeCategory | (LPARAM)Layer::Units);
    for ( auto & subGroup : unitGroup.parsedSubGroups )
        insertUnits(subGroup, hGroupItem);

    AddUnitItems(hGroupItem, unitGroup.items);
}

void UnitTree::InsertAllUnits(HTREEITEM hParent)
{
    unitItems.assign(chkd.scData->units.numUnitTypes(), NULL);
    ChkdProfile* currProfile = chkd.profiles.getCurrProfile();
    if ( currProfile != nullptr && !currProfile->units.customTree.empty() )
    {
        auto & customTree = currProfile->units.customTree;
        for ( const auto & unitGroup : customTree )
            insertUnits(unitGroup, hParent);
    }
    else
    {
        for ( const auto & unitGroup : chkd.scData->units.unitGroups )
            insertUnits(unitGroup, hParent);
    }
}

void UnitTree::AddUnitItems(HTREEITEM hParent, const std::vector<u16> & unitIds)
{
    size_t maxUnits = std::min(unitIds.size(), unitDisplayNames.size());
    for ( size_t i=0; i<maxUnits; i++ )
        unitItems[unitIds[i]] = InsertTreeItem(hParent, unitDisplayNames[unitIds[i]], TreeTypeUnit | (DWORD)unitIds[i]);
}

void UnitTree::AddUnitItems(HTREEITEM hParent, const std::vector<u32> & unitIds)
{
    size_t maxUnits = std::min(unitIds.size(), unitDisplayNames.size());
    for ( size_t i=0; i<maxUnits; i++ )
        unitItems[unitIds[i]] = InsertTreeItem(hParent, unitDisplayNames[unitIds[i]], TreeTypeUnit | (DWORD)unitIds[i]);
}
