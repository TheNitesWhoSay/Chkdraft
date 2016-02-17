#include "PreservedUnitStats.h"
#include "Chkdraft.h"
#include "UnitChange.h"

PreservedUnitStats::PreservedUnitStats() : field(0), numUnits(0), values(nullptr)
{

}

void PreservedUnitStats::flushStats()
{
	field = 0;
	numUnits = 0;
	if ( values != nullptr )
		delete[] values; values = nullptr;
}

void PreservedUnitStats::addStats(Selections &sel, u8 statField)
{
	flushStats();
	field = statField;

	numUnits = sel.numUnits();
	try { values = new u32[numUnits]; }
	catch ( std::bad_alloc ) { flushStats(); return; }
	ChkUnit* unit;
	buffer& UNITS = CM->UNIT();

	u32 i = 0;
	auto &units = sel.getUnits();
	for ( u16 &unitIndex : units )
	{
		u32 pos = ((u32)unitIndex)*UNIT_STRUCT_SIZE + unitFieldLoc[field];
		if ( CM->getUnit(unit, unitIndex) )
		{
			switch ( unitFieldSize[field] )
			{
				case 1: values[i] = (u32)UNITS.get<u8 >(pos); break;
				case 2: values[i] = (u32)UNITS.get<u16>(pos); break;
				case 4: values[i] =		 UNITS.get<u32>(pos); break;
			}
		}
		i++;
	}
}

void PreservedUnitStats::convertToUndo()
{
	if ( numUnits > 0 && values != nullptr )
	{
		// For each selected unit, add the corresponding undo from values
		buffer& units = CM->UNIT();
		u32 pos = 0;

        Selections &selections = CM->GetSelections();
		std::shared_ptr<ReversibleActions> unitChanges(new ReversibleActions);
		u32 i = 0;
		auto &selUnits = selections.getUnits();
		for ( u16 &unitIndex : selUnits )
		{
			pos = ((u32)unitIndex)*UNIT_STRUCT_SIZE + unitFieldLoc[field];
			switch ( unitFieldSize[field] )
			{
				case 1: unitChanges->Insert(std::shared_ptr<UnitChange>(new UnitChange(unitIndex, field, values[i])));
				case 2: unitChanges->Insert(std::shared_ptr<UnitChange>(new UnitChange(unitIndex, field, values[i])));
				case 4: unitChanges->Insert(std::shared_ptr<UnitChange>(new UnitChange(unitIndex, field, values[i])));
			}
			i++;
		}
        CM->AddUndo(unitChanges);
	}
	flushStats();
}
