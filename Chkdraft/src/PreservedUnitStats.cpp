#include "PreservedUnitStats.h"
#include "Chkdraft.h"
#include "UnitChange.h"

PreservedUnitStats::PreservedUnitStats() : field(ChkUnitField::Serial)
{

}

PreservedUnitStats::~PreservedUnitStats()
{

}

void PreservedUnitStats::Clear()
{
    field = ChkUnitField::Serial;
    preservedValues.clear();
}

void PreservedUnitStats::AddStats(Selections &sel, ChkUnitField statField)
{
    Clear();
    field = statField;
    auto &unitIndexes = sel.getUnits();
    for ( u16 &unitIndex : unitIndexes )
    {
        ChkUnit unit = CM->getUnit(unitIndex);
        switch ( field )
        {
            case ChkUnitField::Serial: preservedValues.push_back(unit.serial); break;
            case ChkUnitField::Xc: preservedValues.push_back(unit.xc); break;
            case ChkUnitField::Yc: preservedValues.push_back(unit.yc); break;
            case ChkUnitField::Id: preservedValues.push_back(unit.id); break;
            case ChkUnitField::LinkType: preservedValues.push_back(unit.linkType); break;
            case ChkUnitField::Special: preservedValues.push_back(unit.special); break;
            case ChkUnitField::ValidFlags: preservedValues.push_back(unit.validFlags); break;
            case ChkUnitField::Owner: preservedValues.push_back(unit.owner); break;
            case ChkUnitField::Hitpoints: preservedValues.push_back(unit.hitpoints); break;
            case ChkUnitField::Shields: preservedValues.push_back(unit.shields); break;
            case ChkUnitField::Energy: preservedValues.push_back(unit.energy); break;
            case ChkUnitField::Resources: preservedValues.push_back(unit.resources); break;
            case ChkUnitField::Hanger: preservedValues.push_back(unit.hanger); break;
            case ChkUnitField::StateFlags: preservedValues.push_back(unit.stateFlags); break;
            case ChkUnitField::Unused: preservedValues.push_back(unit.unused); break;
            case ChkUnitField::Link: preservedValues.push_back(unit.link); break;
        }
    }
}

void PreservedUnitStats::convertToUndo()
{
    if ( preservedValues.size() > 0 )
    {
        // For each selected unit, add the corresponding undo from values
        u32 i = 0;

        Selections &selections = CM->GetSelections();
        auto unitChanges = ReversibleActions::Make();
        auto &selectedUnitIndexes = selections.getUnits();
        for ( u16 unitIndex : selectedUnitIndexes )
        {
            unitChanges->Insert(UnitChange::Make(unitIndex, field, preservedValues[i]));
            i++;
        }
        CM->AddUndo(unitChanges);
    }
    Clear();
}
