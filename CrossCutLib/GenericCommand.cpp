#include "GenericCommand.h"
#include "ErrorHandler.h"

GenericCommand::GenericCommand(bool isSynchronous, u32 commandClassId, u32 undoRedoTypeId)
    : isSynchronous(isSynchronous), subCommandsAreAcid(false), commandClassId(commandClassId), undoRedoTypeId(undoRedoTypeId)
{

}

GenericCommand::GenericCommand(const std::vector<GenericCommandPtr> & subCommands, bool isSynchronous, bool subCommandsAreAcid, u32 commandClassId, u32 undoRedoTypeId)
    : subCommands(subCommands), isSynchronous(isSynchronous), subCommandsAreAcid(subCommandsAreAcid), commandClassId(commandClassId), undoRedoTypeId(undoRedoTypeId)
{

}

GenericCommand::~GenericCommand()
{

}

std::string GenericCommand::toString()
{
    return std::string("[Override this method to print command details]");
}

void GenericCommand::Do(Logger & logger)
{
    // Override this method to perform some action
    logger.error("Default Do() hit, this should be overriden!");
}

void GenericCommand::Undo(Logger & logger)
{
    // Override this method to perform some action
    logger.error("Default Undo() hit, this should be overriden!");
}

CommandListener::~CommandListener()
{

}
