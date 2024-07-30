#ifndef UNDOTYPES_H
#define UNDOTYPES_H
#include <common_files/common_files.h>
#include <mapping_core/mapping_core.h>

enum_t(UndoTypes, int32_t, {
    None = 0,
    TerrainChange = 1, // ISOM, TILE, MTXM, DD2, or doodad-linked sprite (specific THG2 entries) change
    UnitChange = 2,
    LocationChange = 3,
    SpriteChange = 4,
    FogChange = 5,
    CutCopyPaste = 6
});

#endif