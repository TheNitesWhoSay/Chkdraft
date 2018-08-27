#ifndef PRESERVEDUNITSTATS_H
#define PRESERVEDUNITSTATS_H
#include "../../../../CommonFiles/CommonFiles.h"
#include "../../../../../MappingCoreLib/MappingCore.h"
#include "../../../../Mapping/Selections.h"

class PreservedUnitStats
{
    public:
        PreservedUnitStats();
        virtual ~PreservedUnitStats();
        void Clear();
        void AddStats(Selections &sel, ChkUnitField statField);
        void convertToUndo();

    private:
        ChkUnitField field;
        std::vector<u32> preservedValues;
};

#endif