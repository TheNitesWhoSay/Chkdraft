#ifndef PRESERVEDUNITSTATS_H
#define PRESERVEDUNITSTATS_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Selections.h"

class PreservedUnitStats
{
	public:
		PreservedUnitStats();
		void flushStats();
		void addStats(SELECTIONS &sel, u8 statField);
		void convertToUndo();

	private:
		u8 field;
		int numUnits;
		u32* values;
};

#endif