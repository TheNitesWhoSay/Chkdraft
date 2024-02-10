#ifndef COMMANDTYPES_H
#define COMMANDTYPES_H
#include "GenericCommand.h"

using u32 = std::uint32_t;

enum class CommandClass : u32
{
    ExampleCommand = (u32)ReservedCommandClassIds::FirstUnreservedCommand
};

enum class UndoRedoType : u32
{
    Example
};

#endif