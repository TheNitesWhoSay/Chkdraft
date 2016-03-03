#ifndef PRESERVEDUNITSTATS_H
#define PRESERVEDUNITSTATS_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "Selections.h"

class PreservedUnitStats
{
    public:
        PreservedUnitStats();
        void flushStats();
        void addStats(Selections &sel, ChkUnitField statField);
        void convertToUndo();

    private:
        ChkUnitField field;
        int numUnits;
        u32* values;
};

#endif