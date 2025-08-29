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
    InsertAllUnits(hUnitRoot);
}

void UnitTree::UpdateUnitNames(const std::vector<std::string> & displayNames)
{
    unitDisplayNames = displayNames;
}

void UnitTree::UpdateUnitName(int unitId)
{
    if ( (size_t)unitId < unitDisplayNames.size() )
        TreeViewControl::SetItemText(UnitItems[unitId], unitDisplayNames.at(unitId));
}

void UnitTree::insertUnits(const Sc::Unit::UnitGroup & unitGroup, HTREEITEM hParent)
{
    HTREEITEM hGroupItem = InsertTreeItem(hParent, unitGroup.groupName, TreeTypeCategory | (LPARAM)Layer::Units);
    for ( auto & subGroup : unitGroup.subGroups )
        insertUnits(subGroup, hGroupItem);

    AddUnitItems(hGroupItem, unitGroup.memberUnits);
}

void UnitTree::InsertAllUnits(HTREEITEM hParent)
{
    for ( const auto & unitGroup : chkd.scData->units.unitGroups )
        insertUnits(unitGroup, hParent);
}

void UnitTree::AddUnitItems(HTREEITEM hParent, const std::vector<u16> & unitIds)
{
    size_t maxUnits = std::min(unitIds.size(), unitDisplayNames.size());
    for ( size_t i=0; i<maxUnits; i++ )
        UnitItems[unitIds[i]] = InsertTreeItem(hParent, unitDisplayNames[unitIds[i]], TreeTypeUnit | (DWORD)unitIds[i]);
}
