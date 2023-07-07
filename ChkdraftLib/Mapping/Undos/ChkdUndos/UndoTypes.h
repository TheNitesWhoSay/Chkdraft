#ifndef UNDOTYPES_H
#define UNDOTYPES_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../MappingCoreLib/MappingCore.h"

enum_t(UndoTypes, int32_t, {
    MtxmChange = 1,
    UnitChange = 2,
    LocationChange = 3,
    IsomChange = 4,
    TileChange = 5
});

#endif