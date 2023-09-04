#ifndef UNDOTYPES_H
#define UNDOTYPES_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../MappingCoreLib/MappingCore.h"

enum_t(UndoTypes, int32_t, {
    TerrainChange = 1, // ISOM, TILE, MTXM, DD2, or doodad-linked sprite (specific THG2 entries) change
    UnitChange = 2,
    LocationChange = 3
});

#endif