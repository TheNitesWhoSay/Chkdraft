#include "PreservedUnitStats.h"
#include "Chkdraft.h"

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

void PreservedUnitStats::addStats(SELECTIONS &sel, u8 statField)
{
	flushStats();
	field = statField;

	UnitNode* currUnit = sel.getFirstUnit();
	numUnits = 0;
	while ( currUnit != nullptr )
	{
		numUnits ++;
		currUnit = currUnit->next;
	}

	currUnit = sel.getFirstUnit();
	try { values = new u32[numUnits]; }
	catch ( std::bad_alloc ) { flushStats(); return; }
	ChkUnit* unit;
	buffer& UNITS = chkd.maps.curr->UNIT();

	for ( int i=0; currUnit != nullptr; i++ )
	{
		u16 index = currUnit->index;
		u32 pos = index*UNIT_STRUCT_SIZE + unitFieldLoc[field];

		if ( chkd.maps.curr->getUnit(unit, index) )
		{
			switch ( unitFieldSize[field] )
			{
				case 1: values[i] = (u32)UNITS.get<u8 >(pos); break;
				case 2: values[i] = (u32)UNITS.get<u16>(pos); break;
				case 4: values[i] =		 UNITS.get<u32>(pos); break;
			}
		}

		currUnit = currUnit->next;
	}
}

void PreservedUnitStats::convertToUndo()
{
	if ( numUnits > 0 && values != nullptr )
	{
		// For each selected unit, add the corresponding undo from values
		UnitNode* currUnit = chkd.maps.curr->selections().getFirstUnit();
		buffer& units = chkd.maps.curr->UNIT();
		u32 pos = 0;

		for ( int i=0; currUnit != nullptr; i++ )
		{
			pos = currUnit->index*UNIT_STRUCT_SIZE + unitFieldLoc[field];
			switch ( unitFieldSize[field] )
			{
				case 1:
					chkd.maps.curr->undos().addUndoUnitChange(currUnit->index, field, values[i]);
					break;
				case 2:
					chkd.maps.curr->undos().addUndoUnitChange(currUnit->index, field, values[i]);
					break;
				case 4:
					chkd.maps.curr->undos().addUndoUnitChange(currUnit->index, field, values[i]);
					break;
			}
			currUnit = currUnit->next;
		}
		chkd.maps.curr->undos().startNext(0);
	}
	flushStats();
}
