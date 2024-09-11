#ifndef PRESERVEDUNITSTATS_H
#define PRESERVEDUNITSTATS_H
#include <common_files/common_files.h>
#include <mapping_core/mapping_core.h>
#include "mapping/selections.h"

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