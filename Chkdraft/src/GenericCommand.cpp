#include "GenericCommand.h"
#include <iostream>
#include <algorithm>
#include <vector>

GenericCommand::GenericCommand(bool isSynchronous) : isCommandSynchronous(isSynchronous), subCommandsAreAcid(false),
    undoRedoTypeId(~((u32)0))
{

}

GenericCommand::GenericCommand(const std::vector<GenericCommandPtr> &subCommands, bool isSynchronous, bool subCommandsAreAcid)
    : subCommands(subCommands), isCommandSynchronous(isSynchronous), subCommandsAreAcid(subCommandsAreAcid), undoRedoTypeId(~((u32)0))
{

}

bool GenericCommand::isSynchronous()
{
    return this->isCommandSynchronous;
}

bool GenericCommand::hasAcidSubCommands()
{
    return subCommandsAreAcid;
}

u32 GenericCommand::Id()
{
    return ~((u32)0);
}

bool GenericCommand::DoCommand()
{
    return DoCommand(false);
}

bool GenericCommand::UndoCommand()
{
    return UndoCommand(false);
}

u32 GenericCommand::GetUndoRedoTypeId()
{
    if ( undoRedoTypeId == GenericCommand::Id() )
        undoRedoTypeId = Id();

    return undoRedoTypeId;
}

void GenericCommand::SetUndoRedoTypeId(u32 undoRedoTypeId)
{
    this->undoRedoTypeId = undoRedoTypeId;
}

bool GenericCommand::DoCommand(bool hasAcidParent)
{
    if ( hasAcidParent || hasAcidSubCommands() )
    {
        if ( subCommands.size() > 0 )
            return DoAcidSubItems();
        else
            return DoAcid();
    }
    else
    {
        if ( subCommands.size() > 0 )
            DoSubItems();
        else
            Do();

        return true;
    }
    return false;
}

bool GenericCommand::UndoCommand(bool hasAcidParent)
{
    if ( hasAcidParent || hasAcidSubCommands() )
    {
        if ( subCommands.size() > 0 )
            return UndoAcidSubItems();
        else
            return UndoAcid();
    }
    else
    {
        if ( subCommands.size() > 0 )
            UndoSubItems();
        else
            Undo();

        return true;
    }
    return false;
}

void GenericCommand::Do()
{
    // Override this method to perform some action
    std::cout << "Default Do() hit, override me!" << std::endl;
}

void GenericCommand::Undo()
{
    // Override this method to perform some action
    std::cout << "Default Undo(), override me!" << std::endl;
}

bool GenericCommand::DoAcid()
{
    // Override this method to perform some action, calls Do() by default
    Do();
    return true;
}

bool GenericCommand::UndoAcid()
{
    // Override this method to perform some action, calls Undo() by default
    Undo();
    return true;
}

void GenericCommand::DoSubItems()
{
    for ( auto subCommand : subCommands )
        subCommand->DoCommand(false);
}

void GenericCommand::UndoSubItems()
{
    for ( auto subCommand : subCommands )
        subCommand->UndoCommand(false);
}

bool GenericCommand::DoAcidSubItems()
{
    for ( auto subCommand = subCommands.begin(); subCommand != subCommands.end(); ++subCommand )
    {
        if ( !(*subCommand)->DoCommand(true) )
        {
            while ( true )
            {
                if ( subCommand == subCommands.begin() )
                    return false;

                --subCommand;

                if ( !(*subCommand)->UndoCommand(true) )
                    throw new AcidRollbackFailure("ACID Do rollback Failed!");
            }
        }
    }
    return true;
}

bool GenericCommand::UndoAcidSubItems()
{
    for ( auto subCommand = subCommands.rbegin(); subCommand != subCommands.rend(); ++subCommand )
    {
        if ( !(*subCommand)->UndoCommand(true) )
        {
            while ( true )
            {
                if ( subCommand == subCommands.rbegin() )
                    return false;

                --subCommand;

                if ( !(*subCommand)->DoCommand(true) )
                    throw new AcidRollbackFailure("ACID Undo Rollback Failed!");
            }
        }
    }
    return true;
}
