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
        void AddStats(Selections & sel, Chk::Unit::Field statField);
        void convertToUndo();

    private:
        Chk::Unit::Field field;
        std::vector<u32> preservedValues;
};

#endif