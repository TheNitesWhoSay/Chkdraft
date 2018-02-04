#ifndef GENERICCOMMAND_H
#define GENERICCOMMAND_H
#include "CommonFiles\CommonFiles.h"
#include "Logger.h"
#include <vector>
#include <queue>

enum class ReservedCommandClassIds : u32
{
    GenericUndoCommand,
    GenericRedoCommand,
    KillCommanderCommand,
    AcidCommandParent,
    FirstUnreservedCommand
};

class Commander;
class CommandListener;
class GenericCommand;

typedef std::shared_ptr<CommandListener> CommandListenerPtr;
typedef std::shared_ptr<GenericCommand> GenericCommandPtr;

class GenericCommand
{
    public:
        GenericCommand(bool isSynchronous, u32 commandClassId, u32 undoRedoTypeId = ~((u32)0));
        GenericCommand(const std::vector<GenericCommandPtr> &subCommands, bool isSynchronous, bool subCommandsAreAcid, u32 commandClassId, u32 undoRedoTypeId = ~((u32)0));

        virtual std::string toString();

    protected:
        friend class Commander;
        virtual void Do(Logger &logger); // Override this method to perform some action when there are no subCommands
        virtual void Undo(Logger &logger); // Override this method to perform some action when there are no subCommands

    private:
        GenericCommand(); // Disallow ctor
        u32 commandClassId;
        u32 undoRedoTypeId;
        bool isSynchronous;
        bool subCommandsAreAcid;
        std::vector<GenericCommandPtr> subCommands;
};

class CommandListener
{
    public:
        virtual void Listen(u32 commandClassId, GenericCommandPtr command) = 0;
};

#endif