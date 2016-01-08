#ifndef UNDOTYPES_H
#define UNDOTYPES_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"

enum class UndoTypes : int32_t
{
	TileChange = 1,
	UnitChange = 2,
	LocationChange = 3
};

#endif