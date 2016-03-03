#include "PreservedUnitStats.h"
#include "Chkdraft.h"
#include "UnitChange.h"

PreservedUnitStats::PreservedUnitStats() : field(ChkUnitField::Serial), numUnits(0), values(nullptr)
{

}

void PreservedUnitStats::flushStats()
{
    field = ChkUnitField::Serial;
    numUnits = 0;
    if ( values != nullptr )
    {
        delete[] values;
        values = nullptr;
    }
}

void PreservedUnitStats::addStats(Selections &sel, ChkUnitField statField)
{
    flushStats();
    field = statField;

    numUnits = sel.numUnits();
    try { values = new u32[numUnits]; }
    catch ( std::bad_alloc ) { flushStats(); return; }

    u32 i = 0;
    auto &units = sel.getUnits();
    for ( u16 &unitIndex : units )
    {
        ChkUnit unit = CM->getUnit(unitIndex);
        switch ( field )
        {
            case ChkUnitField::Serial: values[i] = unit.serial; break;
            case ChkUnitField::Xc: values[i] = unit.xc; break;
            case ChkUnitField::Yc: values[i] = unit.yc; break;
            case ChkUnitField::Id: values[i] = unit.id; break;
            case ChkUnitField::LinkType: values[i] = unit.linkType; break;
            case ChkUnitField::Special: values[i] = unit.special; break;
            case ChkUnitField::ValidFlags: values[i] = unit.validFlags; break;
            case ChkUnitField::Owner: values[i] = unit.owner; break;
            case ChkUnitField::Hitpoints: values[i] = unit.hitpoints; break;
            case ChkUnitField::Shields: values[i] = unit.shields; break;
            case ChkUnitField::Energy: values[i] = unit.energy; break;
            case ChkUnitField::Resources: values[i] = unit.resources; break;
            case ChkUnitField::Hanger: values[i] = unit.hanger; break;
            case ChkUnitField::StateFlags: values[i] = unit.stateFlags; break;
            case ChkUnitField::Unused: values[i] = unit.unused; break;
            case ChkUnitField::Link: values[i] = unit.link; break;
        }
        i++;
    }
}

void PreservedUnitStats::convertToUndo()
{
    if ( numUnits > 0 && values != nullptr )
    {
        // For each selected unit, add the corresponding undo from values
        u32 i = 0;

        Selections &selections = CM->GetSelections();
        std::shared_ptr<ReversibleActions> unitChanges(new ReversibleActions);
        auto &selUnits = selections.getUnits();
        for ( u16 &unitIndex : selUnits )
        {
            unitChanges->Insert(std::shared_ptr<UnitChange>(new UnitChange(unitIndex, field, values[i])));
            i++;
        }
        CM->AddUndo(unitChanges);
    }
    flushStats();
}
