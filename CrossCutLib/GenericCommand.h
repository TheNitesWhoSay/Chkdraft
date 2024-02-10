#ifndef GENERICCOMMAND_H
#define GENERICCOMMAND_H
#include "Logger.h"
#include <vector>

using u32 = std::uint32_t;

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
        GenericCommand(bool isSynchronous, u32 commandClassId, u32 undoRedoTypeId = UINT32_MAX);
        GenericCommand(const std::vector<GenericCommandPtr> & subCommands, bool isSynchronous, bool subCommandsAreAcid, u32 commandClassId, u32 undoRedoTypeId = UINT32_MAX);
        virtual ~GenericCommand();

        virtual std::string toString();

    protected:
        friend class Commander;
        virtual void Do(Logger & logger); // Override this method to perform some action when there are no subCommands
        virtual void Undo(Logger & logger); // Override this method to perform some action when there are no subCommands

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
        virtual ~CommandListener();
};

#endif